from fastapi import FastAPI
from backend.api.lifespan import lifespan
from backend.api.setup import setup_routes, setup_middlewares


app = FastAPI(lifespan=lifespan)
setup_routes(app)
setup_middlewares(app)