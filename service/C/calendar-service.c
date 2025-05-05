#include "../calendar-service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status addTaskToMemoryCalendar(Auth* auth, Task* task, Date date) {
    if (!auth || !auth->userData || !task) {
        return SetStatus(0, "Invalid parameters", "Auth, userData, or task is NULL");
    }
    
    CalendarList* targetNode = NULL;
    error err = findCalendarByDate(&targetNode, auth, date);

    // Check if task time overlaps with existing tasks
    if (err == NULL && targetNode != NULL) {
        TaskList* current = targetNode->calendar.taskList;
        while (current) {
            // Check if new task overlaps with an existing task
            if ((task->setBegin >= current->task.setBegin && task->setBegin < current->task.setEnd) ||
                (task->setEnd > current->task.setBegin && task->setEnd <= current->task.setEnd) ||
                (task->setBegin <= current->task.setBegin && task->setEnd >= current->task.setEnd)) {
                
                // Convert times to strings for error message
                string newBeginStr = NULL, newEndStr = NULL;
                string existingBeginStr = NULL, existingEndStr = NULL;
                
                dateTimeToString(&newBeginStr, task->setBegin);
                dateTimeToString(&newEndStr, task->setEnd);
                dateTimeToString(&existingBeginStr, current->task.setBegin);
                dateTimeToString(&existingEndStr, current->task.setEnd);
                
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), 
                         "Time slot (%s - %s) overlaps with existing task \"%s\" (%s - %s)",
                         newBeginStr, newEndStr, 
                         current->task.title, 
                         existingBeginStr, existingEndStr);
                
                // Free allocated strings
                FreeString(&newBeginStr);
                FreeString(&newEndStr);
                FreeString(&existingBeginStr);
                FreeString(&existingEndStr);
                
                return SetStatus(0, "Time slot overlap", errorMsg);
            }
            current = current->next;
        }
    }

    if (err == NULL && targetNode != NULL) {
        // Calendar found, add task to it
        TaskList* newNode = NULL;
        err = allocateTaskList(&newNode);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate task list node", err);
        }
        
        err = createTask(&newNode->task, task->title, task->location, task->setBegin, task->setEnd, task->dueDate);
        if (err != NULL) {
            free(newNode);
            return SetStatus(0, "Failed to create task copy", err);
        }
        
        // Insert task in chronological order
        if (targetNode->calendar.taskList == NULL) {
            // First task in the calendar
            targetNode->calendar.taskList = newNode;
            newNode->next = NULL;
            newNode->prev = NULL;
        } else {
            // Find proper position based on start time
            TaskList* current = targetNode->calendar.taskList;
            TaskList* prev = NULL;
            
            // Find position to insert based on start time
            while (current && current->task.setBegin <= newNode->task.setBegin) {
                prev = current;
                current = current->next;
            }
            
            if (prev == NULL) {
                // Insert at beginning
                newNode->next = targetNode->calendar.taskList;
                newNode->prev = NULL;
                targetNode->calendar.taskList->prev = newNode;
                targetNode->calendar.taskList = newNode;
            } else if (current == NULL) {
                // Insert at end
                prev->next = newNode;
                newNode->prev = prev;
                newNode->next = NULL;
            } else {
                // Insert in middle
                newNode->next = current;
                newNode->prev = prev;
                prev->next = newNode;
                current->prev = newNode;
            }
        }
    } else {
        // Calendar not found, create new one
        Calendar* newCalendar = NULL;
        err = allocateCalendar(&newCalendar);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate calendar", err);
        }
        
        TaskList* newTaskList = NULL;
        err = allocateTaskList(&newTaskList);
        if (err != NULL) {
            free(newCalendar);
            return SetStatus(0, "Failed to allocate task list", err);
        }
        
        err = createTask(&newTaskList->task, task->title, task->location, task->setBegin, task->setEnd, task->dueDate);
        if (err != NULL) {
            free(newTaskList);
            free(newCalendar);
            return SetStatus(0, "Failed to create task copy", err);
        }
        
        newTaskList->next = NULL;
        newTaskList->prev = NULL;
        
        err = createCalendar(newCalendar, date, newTaskList);
        if (err != NULL) {
            FreeTaskList(newTaskList);
            free(newCalendar);
            return SetStatus(0, "Failed to create calendar", err);
        }
        
        err = addCalendarToList(&auth->userData->calendarList, *newCalendar);
        if (err != NULL) {
            FreeCalendarContent(newCalendar);
            free(newCalendar);
            return SetStatus(0, "Failed to add calendar to list", err);
        }
        
        free(newCalendar);
    }
    
    return SetStatus(1, "Task added to memory calendar successfully", NULL);
}

Status updateCalendarFile(Auth* auth, Date date) {
    if (!auth || !auth->dataPath) {
        return SetStatus(0, "Invalid parameters", "Auth or dataPath is NULL");
    }
    
    // First check if the calendar exists in memory
    CalendarList* targetNode = NULL;
    error err = findCalendarByDate(&targetNode, auth, date);
    if (err != NULL) {
        return SetStatus(0, "Calendar not found in memory", err);
    }
    
    // Find the Calendar directory path
    DataPath* calPath = NULL;
    err = findDataPathByFilename(auth->dataPath, "Calendar", &calPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to find Calendar dataPath", err);
    }
    
    // Convert date to string for filename
    string dateStr = NULL;
    err = dateToString(&dateStr, &date);
    if (err != NULL) {
        return SetStatus(0, "Failed to convert date to string", err);
    }
    
    // Create the filename with extension
    string fullFilename = NULL;
    err = mergeTwoStrings(&fullFilename, dateStr, ".csv");
    if (err != NULL) {
        FreeString(&dateStr);
        return SetStatus(0, "Failed to create filename", err);
    }
    FreeString(&dateStr); 

    // Get the complete file path
    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, calPath->path.path, fullFilename);
    if (err != NULL) {
        FreeString(&fullFilename);
        return SetStatus(0, "Failed to create full path", err);
    }
    
    // printf("Updating file: %s\n", fullPath);
    
    if (FileExist(fullPath)) {
        remove(fullPath);
    }
    
    FILE* file = fopen(fullPath, "w");
    if (!file) {
        FreeString(&fullFilename);
        FreeString(&fullPath);
        return SetStatus(0, "Failed to create calendar file for writing", "File open error");
    }
    
    fprintf(file, "Title,Begin,End,Location\n");
    
    if (targetNode && targetNode->calendar.taskList) {
        TaskList* head = targetNode->calendar.taskList;
        while(head && head->prev) {
            head = head->prev; // Move to the head of the list
        }
        TaskList* current = head;
        
        while (current) {
            string beginStr = NULL, endStr = NULL;
            err = dateTimeToString(&beginStr, current->task.setBegin);
            if (err == NULL) {
                err = dateTimeToString(&endStr, current->task.setEnd);
                if (err == NULL) {
                    fprintf(file, "%s,%s,%s,%s\n",
                            current->task.title,
                            beginStr,
                            endStr,
                            current->task.location);
                }
                FreeString(&beginStr);
            }
            if (endStr != NULL) {
                FreeString(&endStr);
            }
            
            current = current->next;
        }
    }
    
    fclose(file);
    FreeString(&fullFilename);
    FreeString(&fullPath);
    
    return SetStatus(1, "Calendar file updated successfully", NULL);
}

Status LoadCalendarFromFile(Auth* auth, Date date) {
    if (!auth || !auth->userData || !auth->dataPath) {
        return SetStatus(0, "Invalid parameters", "Auth, userData, or dataPath is NULL");
    }
    
    // Check if calendar already exists in memory
    CalendarList* existingCalendar = NULL;
    error err = findCalendarByDate(&existingCalendar, auth, date);
    if (err == NULL && existingCalendar != NULL) {
        return SetStatus(1, "Calendar already loaded in memory", NULL);
    }
    
    // Find the Calendar directory path
    DataPath* calPath = NULL;
    err = findDataPathByFilename(auth->dataPath, "Calendar", &calPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to find Calendar dataPath", err);
    }
    
    // Convert date to string for filename
    string dateStr = NULL;
    err = dateToString(&dateStr, &date);
    if (err != NULL) {
        return SetStatus(0, "Failed to convert date to string", err);
    }
    
    // Create the filename with extension
    string fullFilename = NULL;
    err = mergeTwoStrings(&fullFilename, dateStr, ".csv");
    if (err != NULL) {
        FreeString(&dateStr);
        return SetStatus(0, "Failed to create filename", err);
    }
    FreeString(&dateStr); // Free dateStr as we now have fullFilename

    // Get the complete file path
    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, calPath->path.path, fullFilename);
    if (err != NULL) {
        FreeString(&fullFilename);
        return SetStatus(0, "Failed to create full path", err);
    }
    
    // printf("Loading file: %s\n", fullPath);

    // Check if file exists
    if (!FileExist(fullPath)) {
        FreeString(&fullFilename);
        FreeString(&fullPath);
        return SetStatus(0, "Calendar file doesn't exist", "No calendar for this date");
    }
    
    // Create new calendar
    Calendar newCalendar;
    newCalendar.date = date;
    newCalendar.taskList = NULL;
    
    // Open file for reading
    FILE* file = fopen(fullPath, "r");
    if (!file) {
        FreeString(&fullFilename);
        FreeString(&fullPath);
        return SetStatus(0, "Failed to open calendar file for reading", "File open error");
    }
    
    // Skip the header line
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL) {
        // Empty file, just add empty calendar to list
        fclose(file);
        FreeString(&fullFilename);
        FreeString(&fullPath);
        
        err = addCalendarToList(&auth->userData->calendarList, newCalendar);
        if (err != NULL) {
            return SetStatus(0, "Failed to add calendar to list", err);
        }
        
        return SetStatus(1, "Empty calendar loaded successfully", NULL);
    }
    
    // Read each line and create tasks
    while (fgets(line, sizeof(line), file)) {
        Task* task = NULL;
        err = allocateTask(&task);
        if (err != NULL) {
            Error(err);
            continue;
        }
        
        char title[50], location[50], beginStr[25], endStr[25];
        
        // Parse the line
        if (sscanf(line, "%49[^,],%24[^,],%24[^,],%49[^\n]",
                  title, beginStr, endStr, location) == 4) {
            
            // Allocate and set title
            err = allocateStringLen(&task->title, title, strlen(title));
            if (err != NULL) {
                Error(err);
                free(task);
                continue;
            }
            
            // Allocate and set location
            err = allocateStringLen(&task->location, location, strlen(location));
            if (err != NULL) {
                Error(err);
                FreeString(&task->title);
                free(task);
                continue;
            }
            
            // Parse begin time
            err = stringToDateTime(&task->setBegin, beginStr);
            if (err != NULL) {
                Error(err);
                FreeString(&task->title);
                FreeString(&task->location);
                free(task);
                continue;
            }
            
            // Parse end time
            err = stringToDateTime(&task->setEnd, endStr);
            if (err != NULL) {
                Error(err);
                FreeString(&task->title);
                FreeString(&task->location);
                free(task);
                continue;
            }
            
            // Set due date same as end time
            task->dueDate = task->setEnd;
            
            // Create task list node
            TaskList* newTaskNode = NULL;
            err = allocateTaskList(&newTaskNode);
            if (err != NULL) {
                Error(err);
                FreeTaskContent(task);
                free(task);
                continue;
            }
            
            // Copy task to node and free original task
            newTaskNode->task = *task;
            free(task);
            
            // Add to task list
            if (newCalendar.taskList == NULL) {
                newCalendar.taskList = newTaskNode;
                newTaskNode->next = NULL;
                newTaskNode->prev = NULL;
            } else {
                newTaskNode->next = newCalendar.taskList;
                newTaskNode->prev = NULL;
                newCalendar.taskList->prev = newTaskNode;
                newCalendar.taskList = newTaskNode;
            }
        }
    }
    
    fclose(file);
    FreeString(&fullFilename);
    FreeString(&fullPath);
    
    // Add the calendar to the list
    err = addCalendarToList(&auth->userData->calendarList, newCalendar);
    if (err != NULL) {
        // Free all the tasks we created if we can't add the calendar
        FreeTaskList(newCalendar.taskList);
        return SetStatus(0, "Failed to add calendar to list", err);
    }
    
    return SetStatus(1, "Calendar loaded successfully", NULL);
}


Status removeTaskFromMemoryCalendar(Auth* auth, const string title, Date date) {
    if (!auth || !auth->userData || !title) {
        return SetStatus(0, "Invalid parameters", "Auth, userData, or title is NULL");
    }
    
    CalendarList* targetNode = NULL;
    error err = findCalendarByDate(&targetNode, auth, date);
    if (err != NULL || targetNode == NULL) {
        return SetStatus(0, "Failed to find calendar by date", err ? err : "Calendar not found");
    }
    
    if (!targetNode->calendar.taskList) {
        return SetStatus(0, "No tasks found for this date", "Task list is empty");
    }
    
    TaskList* current = targetNode->calendar.taskList;
    TaskList* prev = NULL;
    code found = 0;
    
    while (current != NULL) {
        // Check if this is the task to remove
        if (current->task.title && strcmp(current->task.title, title) == 0) {
            found = 1;

            if (prev) {
                prev->next = current->next;
            } else {
                targetNode->calendar.taskList = current->next;
            }
            
            if (current->next) {
                current->next->prev = prev;
            }
            
            // Store next pointer before freeing
            TaskList* toFree = current;
            
            // Free the task's string resources
            if (toFree->task.title) {
                FreeString(&toFree->task.title);
            }
            if (toFree->task.location) {
                FreeString(&toFree->task.location);
            }
            
            // Free the node itself
            free(toFree);
            break;
        }
        
        // Move to next node
        prev = current;
        current = current->next;
    }
    
    if (!found) {
        return SetStatus(0, "Task not found", "No task with the specified title found");
    }
    
    if (targetNode->calendar.taskList == NULL) {
        printf("Note: Calendar for this date is now empty\n");
    }
    
    return SetStatus(1, "Task removed from memory successfully", NULL);
}

error findCalendarByDate(CalendarList** calendarNode, Auth* auth, Date date) {
    if (!auth || !auth->userData || !auth->userData->calendarList) {
        return "Auth, userData, or calendarList is NULL";
    }
    
    if (!calendarNode) {
        return "Calendar node pointer is NULL";
    }
    
    CalendarList* current = auth->userData->calendarList;
    while (current) {
        if (isSameDate(&current->calendar.date, &date)) {
            *calendarNode = current;
            return NULL; // Success, no error
        }
        current = current->next;
    }
    
    *calendarNode = NULL; // No matching calendar found
    return "Calendar not found for the specified date";
}