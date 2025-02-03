from fastapi import FastAPI
from gs.backend.api.v1.lifespan import lifespan
from gs.backend.api.v1.setup import setup_routes, setup_middlewares


app = FastAPI(lifespan=lifespan)
setup_routes(app)
setup_middlewares(app)