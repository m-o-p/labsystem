Data storage
============

Course Format
-------------

Course content is stored as a tree of elements. All elements have the following properties:
  1. A title
    * The filename of the elements
  2. A type
    * Specifies the type of this element
Each element type has its own properties.

All element properties are stored in a yaml file called <title>.meta
Elements for normal operation are stored in the content folder. Additional properties can be stored in the secret folder as <title>.secret.
Elements for correctors are stored in the secret folder.

Collections and Assignments contain multiple elements and these are stored in subfolders with the same name as the element.

The node types are:
  1. Display element
    * These are used to show content to the user
    * They have a diplayType property that specifies the way the content is rendered:
      1. HTML
        * Added directly to the page
      2. Text
        * Escaped and added to the page
      3. Markdown
        * Gets converted to HTML
  2. Question element
    * These elements are used to get answers from users and to correct those answers
    * questionType contains the type of the question
      1. Text
        * Free text question for the user
        * Allows file upload for the user (hasFileUpload)
        * Question itself is stored in a display element <title>-Display
        * Offers a hint to the corrector from <title>-Hint
        * Corrector can leave a hint for the user as free text.
        * Can have multiple correction sections that allow more structured feedback via sections
      2. MultipleChoice
        * Automated correction via multiple choice questions
        * Can be single choice
        * Can shuffle the answers
        * Can show hints for the user
          * when a mistake is made
          * when the answer for one checkbox is correct
          * when the answer for one checkbox is incorrect
          * after each mistake
          * can be shuffled to avoid cheating
        * Can allow some mistakes to be made
    3. Collection element
      * Used to group parts of a course
      * Can show or hide their contents when inside another collection
      * Child elements are stored in a subfolder with the same name
    4. Assignments
      * Are collections
      * Used to delimit a major part of a course
      * Can be disabled based on a schedule
      * User rights are based on assignments
      * Determine if the contained elements must be answered by a team or not

A course is a git repository that contains a collection element named "course". This collection can only contain Assignment elements. The secret folder is a subrepository that can be distributed independently.

For each course multiple branches can be accessible at the same time. Each is checked out in a folder, but share the git objets to keep disk usage low. Individual commits can be viewed, but they are not writable. Commit access is slower due to the need to read directly from git objects.
