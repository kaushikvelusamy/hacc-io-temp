Legacy python module
--------------------

.. warning::

   This documentation is for the old python module. It is recommened to use the
   newer, pybind11 based version


The repository includes a genericio Python module that can read
genericio-formatted files and return numpy arrays. This is included in the
standard build. To use it, once you've built genericio, you can read genericio
data as follows:

.. code-block:: bash

   $ export PYTHONPATH=${GENERICIO_DIR}/legacy_python
   $ python


>>> import genericio
>>> genericio.gio_inspect('m000-99.fofproperties')
Number of Elements: 1691
[data type] Variable name
---------------------------------------------
[i 32] fof_halo_count
[i 64] fof_halo_tag
[f 32] fof_halo_mass
[f 32] fof_halo_mean_x
[f 32] fof_halo_mean_y
[f 32] fof_halo_mean_z
[f 32] fof_halo_mean_vx
[f 32] fof_halo_mean_vy
[f 32] fof_halo_mean_vz
[f 32] fof_halo_vel_disp
(i=integer,f=floating point, number bits size)


>>> genericio.gio_read('m000-99.fofproperties','fof_halo_mass')
array([[  4.58575588e+13],
       [  5.00464689e+13],
       [  5.07078771e+12],
       ...,
       [  1.35221006e+13],
       [  5.29125710e+12],
       [  7.12849857e+12]], dtype=float32)