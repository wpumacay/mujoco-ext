import mujoco

import meshcat
import meshcat.geometry as g
import meshcat.transformations as tf

import numpy as np


def rgba_to_hex_int(rgba: np.ndarray) -> int:
    r, g, b, _ = np.uint8(rgba * 255)
    hex_color = (r << 16) + (g << 8) + (b << 0)
    return hex(hex_color)


class MjMeshcatVis:
    def __init__(
        self,
        meshcat_vis: meshcat.Visualizer,
        mjc_model: mujoco.MjModel,
        mjc_data: mujoco.MjData,
    ):
        self._model = mjc_model
        self._data = mjc_data
        self._meshcat_vis = meshcat_vis

        self._buildScene()

    def _buildScene(self) -> None:
        self._root = self._meshcat_vis["mujoco"]
        self._root.delete()

        for geom_idx in range(self._model.ngeom):
            self._createVisGeom(geom_idx)

    def _createVisGeom(self, geom_idx: int) -> None:
        geom_view = self._model.geom(geom_idx)
        geom_type = geom_view.type
        geom_size = geom_view.size.copy()
        geom_color = rgba_to_hex_int(geom_view.rgba.copy())

        geom_shape = None
        if geom_type == mujoco.mjtGeom.mjGEOM_BOX:
            box_extents = 2.0 * geom_size
            geom_shape = g.Box(box_extents)
        elif geom_type == mujoco.mjtGeom.mjGEOM_SPHERE:
            sphere_radius = geom_size[0]
            geom_shape = g.Sphere(sphere_radius)
        elif geom_type == mujoco.mjtGeom.mjGEOM_ELLIPSOID:
            ellipsoid_radii = geom_size
            geom_shape = g.Ellipsoid(geom_size)
        elif geom_type == mujoco.mjtGeom.mjGEOM_CYLINDER:
            cylinder_height = 2.0 * geom_size[1]
            cylinder_radius = geom_size[0]
            geom_shape = g.Cylinder(cylinder_radius, cylinder_height)
        elif geom_type == mujoco.mjtGeom.mjGEOM_CAPSULE:
            capsule_height = 2.0 * geom_size[1]
            capsule_radius = geom_size[0]
            geom_shape = g.Cylinder(capsule_radius, capsule_height)
        else:
            print(
                f'MjMeshcatVis::_createVisGeom >>> geom-type: "{geom_type}" is not supported yet.'
            )

        geom_material = g.MeshPhongMaterial(color=geom_color)
        geom_transform = np.eye(4)
        geom_transform[0:3, 0:3] = self._data.geom_xmat[geom_idx].copy().reshape(3, 3)
        geom_transform[:-1, 3] = self._data.geom_xpos[geom_idx].copy()

        self._root[geom_view.name].set_object(geom_shape, geom_material)
        self._root[geom_view.name].set_transform(geom_transform)

    def _updateVisGeom(self, geom_idx: int) -> None:
        geom_view = self._model.geom(geom_idx)
        geom_transform = np.eye(4)
        geom_transform[:3, :3] = self._data.geom_xmat[geom_idx].copy().reshape(3, 3)
        geom_transform[:-1, 3] = self._data.geom_xpos[geom_idx].copy()
        self._root[geom_view.name].set_transform(geom_transform)

    def render(self) -> None:
        # Update the scene from the current mjData
        for geom_idx in range(self._model.ngeom):
            self._updateVisGeom(geom_idx)
