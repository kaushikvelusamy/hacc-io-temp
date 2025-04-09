The Python Class Interface
==========================
.. currentmodule:: pygio

The :class:`PyGenericIO` python class directly interfaces the C++ ``GenericIO``
class. Writing data is currently not supported through that interface (use
:func:`write_genericio` instead). Note that reading data works without manually
adding variables (:class:`pygio::PyGenericIO::VariableInfo`) to the class instance.
Instead, a list of variable names can be passed to :meth:`PyGenericIO.read`.

.. code-block:: python

   # instantiate a GenericIO class
   gio_file = pygio.PyGenericIO("generic_io_file")

   # inspect (prints to python stdout, also works in notebook)
   gio_file.inspect()

   # get variables
   gio_vars = gio_file.get_variables()

   # print variable names
   for var in gio_vars:
       print(var.name, var.size, var.element_size, var.is_float)

   # read data
   data_all = gio_file.read()
   data_partial = gio_file.read(["x", "y", "z"])

Further methods and members of GenericIO can easly be interfaced by editing
``python/genericio.cpp``.


References
----------

.. autoclass:: PyGenericIO
   :members:

.. autoclass:: pygio::PyGenericIO.VariableInfo
   :members:
   :undoc-members: