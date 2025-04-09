.. GenericIO documentation master file, created by
   sphinx-quickstart on Fri Dec 10 09:52:19 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. only:: latex

   =======================
   GenericIO Documentation
   =======================

.. include:: README.md
   :parser: myst_parser.sphinx_


.. only:: latex

   .. toctree::
      :maxdepth: 3

      self

   .. toctree::
      :caption: Python Interface
      :maxdepth: 2

      python/readwrite
      python/class_interface
      python/mpi
      python/legacy_python

   .. toctree::
      :caption: C++ Interface
      :maxdepth: 2

      cpp/genericio
      cpp/library

   .. toctree::
      :caption: Executables
      :maxdepth: 1

      executables/tools
      executables/benchmarks
      environment
