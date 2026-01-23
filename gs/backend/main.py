from fastapi import FastAPI

from gs.backend.api.backend_setup import setup_logging, setup_middlewares, setup_routes
from gs.backend.api.v1.aro.endpoints.auth.oauth import router as auth_router
from gs.backend.config.config import JWT_SECRET_KEY
from starlette.middleware.sessions import SessionMiddleware
from gs.backend.api.lifespan import lifespan

app = FastAPI(lifespan=lifespan)
# All of our endpoints will be relative to this directory
app.include_router(auth_router, prefix="/api/v1/aro")

# we need this to save temporary code & state in session
app.add_middleware(SessionMiddleware, secret_key=JWT_SECRET_KEY)

setup_logging()
setup_routes(app)
setup_middlewares(app)
