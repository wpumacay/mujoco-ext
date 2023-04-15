import os
import sys

import mujoco


class MeshcatVisualizer:
    def __init__(self, mjc_model, mjc_data):
        self._model = mjc_model
        self._data = mjc_data

    def render(self) -> None:
        ...
