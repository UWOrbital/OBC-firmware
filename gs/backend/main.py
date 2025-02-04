from fastapi import FastAPI

from gs.backend.api.lifespan import lifespan
from gs.backend.api.setup import setup_middlewares, setup_routes

app = FastAPI(lifespan=lifespan)
setup_routes(app)
setup_middlewares(app)
