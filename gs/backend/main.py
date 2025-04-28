from fastapi import FastAPI

from gs.backend.api.backend_setup import setup_configurator, setup_middlewares, setup_routes
from gs.backend.api.lifespan import lifespan

app = FastAPI(lifespan=lifespan)
setup_configurator(app)
setup_routes(app)
setup_middlewares(app)
