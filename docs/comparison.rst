Comparison
----------

========================= ============= =================== ===================
Feature                   Existing      Rewrite             Notes
========================= ============= =================== ===================
Versioning                Yes           With Git
Editing history           No            Git commits         Commits have
                                                            message and author
Course editing            Yes           Yes
Multiple versions         Separate      Git branches in
                          servers       separate
                                        worktrees
Merging of changes        No            Yes, limited to
                                        git merge
Course distribution       Import/Export Git repositories
Distributing changes      No            Git commits
Course management         Yes           More limited
Assignments               Hard coded    Flexible nesting
Semesters                 Yes           Not implemented
Email notifications       Yes           Not implemented
User management           Yes           Yes                 Using DB
Permissions               Matrix        Role based          Using DB
                                        Flexible
Answer history            Yes           Yes                 Using DB
File upload               Yes           Only answers
Language                  PHP           Python
Database support          MySQL         ORM allows many     Rewrite uses
                                                            only one database
HTML templates            String concat Jinja2
View templates            Substitution  Python builtin      Currently same
                                        Can be extended     functionality
Scheduling                Yes           Yes, UI not
                                        intuitive.
                          Schedule is   Schedule in DB
                          an element
Elements can be extended  Yes           Yes                 Python has simpler
                                                            classes
Text question correction  Yes           Also has hints
Multple choice questions  Yes           Also allows teams
                                        to answer
Score reporting           Yes           Limited
Editing outside UI        Difficult via Human readable
                          Import/Export text files in git
Dependency management     Shell scripts pip
Execution environment     Apache        WSGI/Flask
Scalability problems      Database      Database + Git
Translations              Yes           Yes, with dedicated
                                        tooling
========================= ============= =================== ===================
