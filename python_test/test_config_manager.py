import importlib

from gs.backend.config import config
from gs.backend.config.cors_config import CORSConfig
from gs.backend.config.logger_config import LoggerConfig


def test_logger_config_default():
    cfg = LoggerConfig()
    assert cfg.excluded_endpoints == []


def test_cors_config_default():
    cfg = CORSConfig()

    assert cfg.allow_origins == ["http://localhost:5173"]
    assert cfg.allow_credentials == True
    assert cfg.allow_methods == ["*"]
    assert cfg.allow_headers == ["*"]


def test_backend_configuration_from_env(monkeypatch):
    monkeypatch.setenv("LOGGER_EXCLUDED_ENDPOINTS", '["/test"]')
    monkeypatch.setenv("CORS_ALLOW_ORIGINS", '["http://test.com"]')

    importlib.reload(config)
    cfg = config.backend_config
    assert "/test" in cfg.logger_config.excluded_endpoints
    assert "http://test.com" in cfg.cors_config.allow_origins
