from fastapi import FastAPI

from gs.backend.api.backend_setup import setup_middlewares, setup_routes
from gs.backend.api.lifespan import lifespan

app = FastAPI(lifespan=lifespan)
setup_routes(app)
setup_middlewares(app)
setup_exception_handlers(app)
