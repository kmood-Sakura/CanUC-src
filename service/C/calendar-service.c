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

    if (err == NULL && targetNode != NULL) {
        TaskList* current = targetNode->calendar.taskList;
        while (current) {
            if ((task->setBegin >= current->task.setBegin && task->setBegin < current->task.setEnd) ||
                (task->setEnd > current->task.setBegin && task->setEnd <= current->task.setEnd) ||
                (task->setBegin <= current->task.setBegin && task->setEnd >= current->task.setEnd)) {
                
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
        
        if (targetNode->calendar.taskList == NULL) {
            targetNode->calendar.taskList = newNode;
            newNode->next = NULL;
            newNode->prev = NULL;
        } else {
            TaskList* current = targetNode->calendar.taskList;
            TaskList* prev = NULL;
            
            while (current && current->task.setBegin <= newNode->task.setBegin) {
                prev = current;
                current = current->next;
            }
            
            if (prev == NULL) {
                newNode->next = targetNode->calendar.taskList;
                newNode->prev = NULL;
                targetNode->calendar.taskList->prev = newNode;
                targetNode->calendar.taskList = newNode;
            } else if (current == NULL) {
                prev->next = newNode;
                newNode->prev = prev;
                newNode->next = NULL;
            } else {
                newNode->next = current;
                newNode->prev = prev;
                prev->next = newNode;
                current->prev = newNode;
            }
        }
    } else {
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
    
    CalendarList* targetNode = NULL;
    error err = findCalendarByDate(&targetNode, auth, date);
    if (err != NULL) {
        return SetStatus(0, "Calendar not found in memory", err);
    }
    
    DataPath* calPath = NULL;
    err = findDataPathByFilename(auth->dataPath, "Calendar", &calPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to find Calendar dataPath", err);
    }
    
    string dateStr = NULL;
    err = dateToString(&dateStr, &date);
    if (err != NULL) {
        return SetStatus(0, "Failed to convert date to string", err);
    }
    
    string fullFilename = NULL;
    err = mergeTwoStrings(&fullFilename, dateStr, ".csv");
    if (err != NULL) {
        FreeString(&dateStr);
        return SetStatus(0, "Failed to create filename", err);
    }
    FreeString(&dateStr); 

    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, calPath->path.path, fullFilename);
    if (err != NULL) {
        FreeString(&fullFilename);
        return SetStatus(0, "Failed to create full path", err);
    }
    
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
            head = head->prev;
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
    
    CalendarList* existingCalendar = NULL;
    error err = findCalendarByDate(&existingCalendar, auth, date);
    if (err == NULL && existingCalendar != NULL) {
        return SetStatus(1, "Calendar already loaded in memory", NULL);
    }
    
    DataPath* calPath = NULL;
    err = findDataPathByFilename(auth->dataPath, "Calendar", &calPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to find Calendar dataPath", err);
    }
    
    string dateStr = NULL;
    err = dateToString(&dateStr, &date);
    if (err != NULL) {
        return SetStatus(0, "Failed to convert date to string", err);
    }
    
    string fullFilename = NULL;
    err = mergeTwoStrings(&fullFilename, dateStr, ".csv");
    if (err != NULL) {
        FreeString(&dateStr);
        return SetStatus(0, "Failed to create filename", err);
    }
    FreeString(&dateStr);

    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, calPath->path.path, fullFilename);
    if (err != NULL) {
        FreeString(&fullFilename);
        return SetStatus(0, "Failed to create full path", err);
    }

    if (!FileExist(fullPath)) {
        FreeString(&fullFilename);
        FreeString(&fullPath);
        return SetStatus(0, "Calendar file doesn't exist", "No calendar for this date");
    }
    
    Calendar newCalendar;
    newCalendar.date = date;
    newCalendar.taskList = NULL;
    
    FILE* file = fopen(fullPath, "r");
    if (!file) {
        FreeString(&fullFilename);
        FreeString(&fullPath);
        return SetStatus(0, "Failed to open calendar file for reading", "File open error");
    }
    
    char line[256];
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        FreeString(&fullFilename);
        FreeString(&fullPath);
        
        err = addCalendarToList(&auth->userData->calendarList, newCalendar);
        if (err != NULL) {
            return SetStatus(0, "Failed to add calendar to list", err);
        }
        
        return SetStatus(1, "Empty calendar loaded successfully", NULL);
    }
    
    while (fgets(line, sizeof(line), file)) {
        Task* task = NULL;
        err = allocateTask(&task);
        if (err != NULL) {
            Error(err);
            continue;
        }
        
        char title[50], location[50], beginStr[25], endStr[25];
        
        if (sscanf(line, "%49[^,],%24[^,],%24[^,],%49[^\n]",
                  title, beginStr, endStr, location) == 4) {
            
            err = allocateStringLen(&task->title, title, strlen(title));
            if (err != NULL) {
                Error(err);
                free(task);
                continue;
            }
            
            err = allocateStringLen(&task->location, location, strlen(location));
            if (err != NULL) {
                Error(err);
                FreeString(&task->title);
                free(task);
                continue;
            }
            
            err = stringToDateTime(&task->setBegin, beginStr);
            if (err != NULL) {
                Error(err);
                FreeString(&task->title);
                FreeString(&task->location);
                free(task);
                continue;
            }
            
            err = stringToDateTime(&task->setEnd, endStr);
            if (err != NULL) {
                Error(err);
                FreeString(&task->title);
                FreeString(&task->location);
                free(task);
                continue;
            }
            
            task->dueDate = task->setEnd;
            
            TaskList* newTaskNode = NULL;
            err = allocateTaskList(&newTaskNode);
            if (err != NULL) {
                Error(err);
                FreeTaskContent(task);
                free(task);
                continue;
            }
            
            newTaskNode->task = *task;
            free(task);
            
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
    
    err = addCalendarToList(&auth->userData->calendarList, newCalendar);
    if (err != NULL) {
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
            
            TaskList* toFree = current;
            
            if (toFree->task.title) {
                FreeString(&toFree->task.title);
            }
            if (toFree->task.location) {
                FreeString(&toFree->task.location);
            }
            
            free(toFree);
            break;
        }
        
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
            return NULL;
        }
        current = current->next;
    }
    
    *calendarNode = NULL;
    return "Calendar not found for the specified date";
}
