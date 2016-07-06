Workflow
========

Creating a new course
---------------------

1. User inputs name
2. Folder is created and populated with initial data
3. Git repository is initialized
4. The initial data is commited to git
5. User is redirected to the new course

Viewing an element
------------------

The url has 3 important parameters:

1. The course
2. The branch
3. The path within the course

The path to the actual file is generated based on these components and the following is performed:

1. Check permissions
2. Check if branch is a commit
2.1. If branch then read the file from the filesystem
2.2. If commit then read file contents from git
3. Render the template. The template may access other files and goes trough the same workflow.
4. Display the template

Creating an element
-------------------

1. The user selects edit for the cointainer of the new element
2. The user selects the option to create a new element of the appropriate type
3.1 Display and Assignment elements can be edited before creation
3.2 Questions:

  1. Choose a new name for the element
  2. Element with default data is created
  3. Redirect user to the editing form of the element

Deleting an element
-------------------

1. User selects element to delete
2. Element is removed from its parent
3. Element metadata file is delete_head
4, Subelements are deleted

Viewing a Question element
--------------------------

Question elements reads data from the database between steps 2 and 3:

1. Get or create a entry in the *Answer* table
2. Get the latest *AnswerContent* entry if it exists
3. Forward the information to the template

The template handles the display of answer and correction information.

Answering a Question element
----------------------------

Viewing a question element also generates the form required to answer the question.

1. User inputs answer
2. Create a new *AnswerContent* entry with the user input
3. Redirect user to the question

Correcting a MultipleChoice question
------------------------------------

Correction happens automatically during answering

If answering alone:

1. Check if answering is allowed
2. Check correctness
3. Fill in correction into the *AnswerContent* entry

If answering as a team:

1. Check if answering is allowed
2. Check if all team members have answered
3. Check if all answers coincide
4. Create a new *AnswerContent* entry for the entire teamwork
5. Fill in correction into the *AnswerContent* entry

Correcting a Text question
--------------------------

Correction is performed by an user with the appropriate permissions.

1. Fill in the correction
2. Fill in correction into the *AnswerContent* entry

Editing an element
------------------

1. User edits the element using the form
2. Check if editing is allowed

  * User must have permission
  * Edit is performed on a branch, not a commit

2. Validate the input data
3. Store the data in the filesystem

Changing an elements path
-------------------------

If an edit changes the path of an element more work needs to be performed:

1. Element metadata is moved
2. Element is removed from its old parent
3. The new parent adds the element
4. All subelements of this element are moved

Commiting changes
-----------------

1. User fills in message and commits
2. Files are added to git index
3. New commit is created using the message and with an appropriate author

Creating a branch
-----------------

1. User fills in the branch name
2. A new git worktree is created using the branch name
3. The previous branch is checked out in the worktree
4. User is redirected to the new branch

Merging branches
----------------

1. User navigates to the branch receiving the merge
2. User selects the second branch to be merged
3. A git merge is performed

If conflicts occur, they must be resolved manually

Creating an user
----------------

1. Administrator fills in the form for the new user
2. User is added to the database, but has no permission

Creating a role
---------------

1. Administrator chooses a name and the permissions associated with the role
2. Role is added to the database and can be assigned to users

Example roles:

* User with the *read* and *answer* permissions
* Corrector with the *read* and *correct* permissions

Assigning roles to users
------------------------

A user has an unlimited number of roles and can even be assigned the same role multiple times. The role can always be active, but it can also be restricted in the following ways:

1. *Schedule* sets a time constraint for the assignment
2. *Course* allows the role to be restricted to a specific course
3. *Assignment* allows the role to be restricted to a specific assignment

Assignment of roles to users is simple:

1. Navigate to the user and choose to assign a new role
2. Select the role and the restrictions
3. Role is assigned to the user

Checking permissions
--------------------

Permissions are checked according to the following workflows:

If the permission relates to an element:

1. Get all roles of the user
2. Remove all roles restricted by the Schedule
3. Remove all roles restricted by the Assignment
4. Remove all roles restricted by the Course
5. For each remaining role check if it has the permission
6. If any role has the permission grant access, deny otherwise

If the permission relates to an entire course:

1. Get all roles of the user
2. Remove all roles restricted by the Schedule
3. Remove all roles restricted by the Course
4. For each remaining role check if it has the permission
5. If any role has the permission grant access, deny otherwise

If the permission is global:

1. Get all roles of the user
2. Remove all roles restricted by the Schedule
4. For each remaining role check if it has the permission
5. If any role has the permission grant access, deny otherwise
