#include "../load-user-data.h"

#include <stdio.h>
#include <stdlib.h>

Status* LoadUserDataAPI(Auth* auth) {
    if (!verifyAuth(auth)) {
        return createStatus(0,"Auth required",NULL); // Invalid auth
    }
    if (auth->userData != NULL) {
        FreeUserData(auth->userData); // Free existing user data
        auth->userData = NULL; // Reset user data pointer
        // reload user data
    }
    auth->userData = (UserData*)malloc(sizeof(UserData));
    if (auth->userData == NULL) {
        return createStatus(0,"Memory allocation failed",NULL); // Memory allocation failed
    }
    
    //temperary data
    DateTime tempDateTime = getCurrentDateTime();
    auth->userData->notifications = createNotification("temp header","temp title", tempDateTime); // create empty notification
    auth->userData->calendars = createCalendar(*getCurrentDate(),createTask("temp title","temp location",tempDateTime,tempDateTime,tempDateTime)); // create empty calendar
    auth->userData->leb2 = createLEB2(
        createSemester(
            createClass(
                createDashboard(
                    "temp description"
                ),
                createSyllabus(
                    "temp description"
                ),
                createAssignmentActivity(
                    createAssignment(
                        "temp header",
                        "temp description",
                        tempDateTime,
                        tempDateTime
                    )
                ),
                createLearningActivity(
                    "temp name",
                    "temp description",
                    "temp url"
                ),
                createAttendance(
                    "temp name",
                    "temp description",
                    "temp img url"
                ),
                createScoreBook(
                    "temp name",
                    "temp description",
                    "temp img url"
                ),
                createLearnIt(
                    "temp name",
                    "temp description",
                    "temp img url"
                ),
                createSurvey(
                    "temp name",
                    "temp description",
                    "temp img url"
                ),
                createFile(
                    "temp filename",
                    "temp file dir",
                    "temp file type"
                ),
                createMembers(
                    "temp name",
                    "temp description",
                    "temp img url"
                )
            )
        )
    );

    return createStatus(1,"Load user data successfully",NULL); // Success
}