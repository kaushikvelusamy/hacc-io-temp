#                    Copyright (C) 2015, UChicago Argonne, LLC
#                               All Rights Reserved
#
#                               Generic IO (ANL-15-066)
#                     Hal Finkel, Argonne National Laboratory
#
#                              OPEN SOURCE LICENSE
#
# Under the terms of Contract No. DE-AC02-06CH11357 with UChicago Argonne,
# LLC, the U.S. Government retains certain rights in this software.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#   3. Neither the names of UChicago Argonne, LLC or the Department of Energy
#      nor the names of its contributors may be used to endorse or promote
#      products derived from this software without specific prior written
#      permission.
#
# *****************************************************************************
#
#                                  DISCLAIMER
# THE SOFTWARE IS SUPPLIED “AS IS” WITHOUT WARRANTY OF ANY KIND.  NEITHER THE
# UNTED STATES GOVERNMENT, NOR THE UNITED STATES DEPARTMENT OF ENERGY, NOR
# UCHICAGO ARGONNE, LLC, NOR ANY OF THEIR EMPLOYEES, MAKES ANY WARRANTY,
# EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE
# ACCURACY, COMPLETENESS, OR USEFULNESS OF ANY INFORMATION, DATA, APPARATUS,
# PRODUCT, OR PROCESS DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE
# PRIVATELY OWNED RIGHTS.
#
# *****************************************************************************

CC = icx
CXX = icpx

MPICC = mpicc
MPICXX = mpicxx

all: fe-progs mpi-progs
sql: fe-sqlite

BLOSC_CPPFLAGS := \
	-Ithirdparty/blosc \
	-DHAVE_LZ4 -DHAVE_SNAPPY -DHAVE_ZLIB -DHAVE_ZSTD -DHAVE_UNISTD_H \
	-Ithirdparty/blosc/internal-complibs/zlib-1.2.8 \
	-Ithirdparty/blosc/internal-complibs/lz4-1.7.2 \
	-Ithirdparty/blosc/internal-complibs/snappy-1.1.1 \
	-Ithirdparty/blosc/internal-complibs/zstd-0.7.4 \
	-Ithirdparty/blosc/internal-complibs/zstd-0.7.4/legacy \
	-Ithirdparty/blosc/internal-complibs/zstd-0.7.4/compress \
	-Ithirdparty/blosc/internal-complibs/zstd-0.7.4/common \
	-Ithirdparty/blosc/internal-complibs/zstd-0.7.4/dictBuilder \
	-Ithirdparty/blosc/internal-complibs/zstd-0.7.4/decompress \
	-Ithirdparty/SZ/sz/include

# Set the VELOC_INSTALL_DIR environment variable to enable VELOC support
ifdef VELOC_INSTALL_DIR
BLOSC_CPPFLAGS := $(BLOSC_CPPFLAGS) \
	-DGENERICIO_WITH_VELOC \
	-Ithirdparty/veloc \
	-I$(VELOC_INSTALL_DIR)/include
BASE_CXXFLAGS := \
	-L$(VELOC_INSTALL_DIR)/lib -L$(VELOC_INSTALL_DIR)/lib64 \
	-Wl,-rpath,$(VELOC_INSTALL_DIR)/lib64 -Wl,-rpath,$(VELOC_INSTALL_DIR)/lib\
	-Wl,--copy-dt-needed-entries -lveloc-client
endif

BASE_CPPFLAGS := $(BLOSC_CPPFLAGS) -I. -D__STDC_CONSTANT_MACROS

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OPENMPFLAG=-fopenmp
endif

# Use gnu99 only on Linux
# (ideally we would trigger on compiler rather than platform)
ifeq ($(UNAME_S),Linux)
    G99FLAG=-std=gnu99
endif

FEDIR = frontend
FE_CFLAGS := -g -fPIC -O3 $(OPENMPFLAG) $(G99FLAG)
FE_CXXFLAGS := -g -fPIC -O3 $(OPENMPFLAG) $(BASE_CXXFLAGS)
FE_CPPFLAGS := $(BASE_CPPFLAGS) -Ithirdparty/sqlite -DGENERICIO_NO_MPI

MPIDIR = mpi
MPI_CFLAGS := -g -O3 $(OPENMPFLAG) $(G99FLAG)
MPI_CXXFLAGS := -g -O3 $(OPENMPFLAG) $(BASE_CXXFLAGS)
MPI_CPPFLAGS := $(BASE_CPPFLAGS)

$(FEDIR):
	mkdir -p $(FEDIR)

$(FEDIR)/%.o: %.c | $(FEDIR)
	mkdir -p $(dir $@)
	$(CC) $(FE_CFLAGS) $(FE_CPPFLAGS) -c -o $@ $<

$(FEDIR)/%.o: %.cxx | $(FEDIR)
	mkdir -p $(dir $@)
	$(CXX) $(FE_CXXFLAGS) $(FE_CPPFLAGS) -c -o $@ $<

BLOSC_O := \
	thirdparty/blosc/blosc.o \
	thirdparty/blosc/blosclz.o \
	thirdparty/blosc/shuffle.o \
	thirdparty/blosc/bitshuffle-generic.o \
	thirdparty/blosc/shuffle-generic.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/gzwrite.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/crc32.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/inffast.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/zutil.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/infback.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/deflate.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/inflate.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/gzread.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/gzlib.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/gzclose.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/uncompr.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/compress.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/inftrees.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/trees.o \
	thirdparty/blosc/internal-complibs/zlib-1.2.8/adler32.o \
	thirdparty/blosc/internal-complibs/lz4-1.7.2/lz4.o \
	thirdparty/blosc/internal-complibs/lz4-1.7.2/lz4hc.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/legacy/zstd_v01.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/legacy/zstd_v02.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/legacy/zstd_v03.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/legacy/zstd_v06.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/legacy/zstd_v04.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/legacy/zstd_v05.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/compress/fse_compress.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/compress/zstd_compress.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/compress/huf_compress.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/compress/zbuff_compress.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/common/entropy_common.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/common/xxhash.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/common/zstd_common.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/common/fse_decompress.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/dictBuilder/zdict.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/dictBuilder/divsufsort.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/decompress/zstd_decompress.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/decompress/huf_decompress.o \
	thirdparty/blosc/internal-complibs/zstd-0.7.4/decompress/zbuff_decompress.o \
	thirdparty/blosc/internal-complibs/snappy-1.1.1/snappy-c.o \
	thirdparty/blosc/internal-complibs/snappy-1.1.1/snappy.o \
	thirdparty/blosc/internal-complibs/snappy-1.1.1/snappy-sinksource.o \
	thirdparty/blosc/internal-complibs/snappy-1.1.1/snappy-stubs-internal.o \
	thirdparty/SZ/sz/src/ByteToolkit.o \
	thirdparty/SZ/sz/src/dataCompression.o \
	thirdparty/SZ/sz/src/DynamicIntArray.o \
	thirdparty/SZ/sz/src/iniparser.o \
	thirdparty/SZ/sz/src/CompressElement.o \
	thirdparty/SZ/sz/src/DynamicByteArray.o \
	thirdparty/SZ/sz/src/rw.o \
	thirdparty/SZ/sz/src/TightDataPointStorageI.o \
	thirdparty/SZ/sz/src/TightDataPointStorageD.o \
	thirdparty/SZ/sz/src/TightDataPointStorageF.o \
	thirdparty/SZ/sz/src/conf.o \
	thirdparty/SZ/sz/src/DynamicDoubleArray.o \
	thirdparty/SZ/sz/src/TypeManager.o \
	thirdparty/SZ/sz/src/dictionary.o \
	thirdparty/SZ/sz/src/DynamicFloatArray.o \
	thirdparty/SZ/sz/src/VarSet.o \
	thirdparty/SZ/sz/src/callZlib.o \
	thirdparty/SZ/sz/src/Huffman.o \
	thirdparty/SZ/sz/src/sz_float.o \
	thirdparty/SZ/sz/src/sz_double.o \
	thirdparty/SZ/sz/src/sz_int8.o \
	thirdparty/SZ/sz/src/sz_int16.o \
	thirdparty/SZ/sz/src/sz_int32.o \
	thirdparty/SZ/sz/src/sz_int64.o \
	thirdparty/SZ/sz/src/sz_uint8.o \
	thirdparty/SZ/sz/src/sz_uint16.o \
	thirdparty/SZ/sz/src/sz_uint32.o \
	thirdparty/SZ/sz/src/sz_uint64.o \
	thirdparty/SZ/sz/src/szd_uint8.o \
	thirdparty/SZ/sz/src/szd_uint16.o \
	thirdparty/SZ/sz/src/szd_uint32.o \
	thirdparty/SZ/sz/src/szd_uint64.o \
	thirdparty/SZ/sz/src/szd_float.o \
	thirdparty/SZ/sz/src/szd_double.o \
	thirdparty/SZ/sz/src/szd_int8.o \
	thirdparty/SZ/sz/src/szd_int16.o \
	thirdparty/SZ/sz/src/szd_int32.o \
	thirdparty/SZ/sz/src/szd_int64.o \
	thirdparty/SZ/sz/src/utility.o \
	thirdparty/SZ/sz/src/sz.o \
	thirdparty/SZ/sz/src/sz_float_pwr.o \
	thirdparty/SZ/sz/src/sz_double_pwr.o \
	thirdparty/SZ/sz/src/szd_float_pwr.o \
	thirdparty/SZ/sz/src/szd_double_pwr.o \
	thirdparty/SZ/sz/src/sz_double_ts.o \
	thirdparty/SZ/sz/src/sz_float_ts.o \
	thirdparty/SZ/sz/src/szd_double_ts.o \
	thirdparty/SZ/sz/src/szd_float_ts.o

ifdef VELOC_INSTALL_DIR
BLOSC_O := $(BLOSC_O) \
	thirdparty/veloc/FileIOVELOC.o
endif

FE_BLOSC_O := $(addprefix $(FEDIR)/,$(BLOSC_O))

$(FEDIR)/GenericIOPrint: $(FEDIR)/GenericIOPrint.o $(FEDIR)/GenericIO.o $(FE_BLOSC_O)
	$(CXX) $(FE_CXXFLAGS) -o $@ $^

$(FEDIR)/GenericIOVerify: $(FEDIR)/GenericIOVerify.o $(FEDIR)/GenericIO.o $(FE_BLOSC_O)
	$(CXX) $(FE_CXXFLAGS) -o $@ $^

FE_UNAME := $(shell uname -s)
ifeq ($(FE_UNAME),Darwin)
FE_SHARED := -bundle
else
FE_SHARED := -shared
endif

$(FEDIR)/libpygio.so: $(FEDIR)/GenericIO.o $(FEDIR)/legacy_python/lib/gio.o $(FE_BLOSC_O)
	$(CXX) $(FE_CXXFLAGS) $(FE_SHARED) -o $@ $^

$(FEDIR)/GenericIOSQLite.so: $(FEDIR)/GenericIOSQLite.o $(FEDIR)/GenericIO.o $(FE_BLOSC_O)
	$(CXX) $(FE_CXXFLAGS) $(FE_SHARED) -o $@ $^

SQLITE_CPPFLAGS := -DSQLITE_ENABLE_COLUMN_METADATA=1 -DSQLITE_DISABLE_DIRSYNC=1 -DSQLITE_ENABLE_FTS3=3 -DSQLITE_ENABLE_RTREE=1 -DSQLITE_ENABLE_UNLOCK_NOTIFY=1 -DSQLITE_ENABLE_LOAD_EXTENSION=1 -DHAVE_READLINE=1

$(FEDIR)/sqbuild:
	mkdir -p $(FEDIR)/sqbuild

$(FEDIR)/sqbuild/%.o: thirdparty/sqlite/%.c | $(FEDIR)/sqbuild
	$(CC) $(FE_CFLAGS) $(FE_CPPFLAGS) $(SQLITE_CPPFLAGS) -c -o $@ $<

$(FEDIR)/sqlite3: $(FEDIR)/sqbuild/sqlite3.o $(FEDIR)/sqbuild/shell.o
	$(CC) $(FE_CFLAGS) -o $@ $^ -pthread -lreadline -lrt -ldl

$(MPIDIR):
	mkdir -p $(MPIDIR)

$(MPIDIR)/%.o: %.c | $(MPIDIR)
	mkdir -p $(dir $@)
	$(MPICC) $(MPI_CFLAGS) $(MPI_CPPFLAGS) -c -o $@ $<

$(MPIDIR)/%.o: %.cxx | $(MPIDIR)
	mkdir -p $(dir $@)
	$(MPICXX) $(MPI_CXXFLAGS) $(MPI_CPPFLAGS) -c -o $@ $<

MPI_BLOSC_O := $(addprefix $(MPIDIR)/,$(BLOSC_O))

$(MPIDIR)/GenericIOPrint: $(MPIDIR)/GenericIOPrint.o $(MPIDIR)/GenericIO.o $(MPI_BLOSC_O)
	$(MPICXX) $(MPI_CXXFLAGS) -o $@ $^

$(MPIDIR)/GenericIOVerify: $(MPIDIR)/GenericIOVerify.o $(MPIDIR)/GenericIO.o $(MPI_BLOSC_O)
	$(MPICXX) $(MPI_CXXFLAGS) -o $@ $^

$(MPIDIR)/GenericIOBenchmarkRead: $(MPIDIR)/GenericIOBenchmarkRead.o $(MPIDIR)/GenericIO.o $(MPI_BLOSC_O)
	$(MPICXX) $(MPI_CXXFLAGS) -o $@ $^

$(MPIDIR)/GenericIOBenchmarkWrite: $(MPIDIR)/GenericIOBenchmarkWrite.o $(MPIDIR)/GenericIO.o $(MPI_BLOSC_O)
	$(MPICXX) $(MPI_CXXFLAGS) -o $@ $^

$(MPIDIR)/GenericIORewrite: $(MPIDIR)/GenericIORewrite.o $(MPIDIR)/GenericIO.o $(MPI_BLOSC_O)
	$(MPICXX) $(MPI_CXXFLAGS) -o $@ $^

frontend-progs: $(FEDIR)/GenericIOPrint $(FEDIR)/GenericIOVerify $(FEDIR)/libpygio.so
fe-progs: frontend-progs

mpi-progs: $(MPIDIR)/GenericIOPrint $(MPIDIR)/GenericIOVerify $(MPIDIR)/GenericIOBenchmarkRead $(MPIDIR)/GenericIOBenchmarkWrite $(MPIDIR)/GenericIORewrite

frontend-sqlite: $(FEDIR)/GenericIOSQLite.so $(FEDIR)/sqlite3
fe-sqlite: frontend-sqlite

clean:
	rm -rf frontend mpi legacy_python/genericio.pyc
