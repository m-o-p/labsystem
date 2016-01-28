from app import app
from entities import create_tables

create_tables()

import routes

app.run()
