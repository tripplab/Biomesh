!INCLUDE ..\BuildFlags.mk

SRCDIR=..\..\..\source

CPPFLAGS=$(CPPFLAGS) /I$(SRCDIR) /I$(SRCDIR)\METIS /I$(SRCDIR)\Tools\FEMSolver $(MPI_INCLUDE)
LDFLAGS=$(LDFLAGS) ..\FEMT.lib ..\METIS.lib $(MPI_LIBS)

EXE_PREFIX=..\..\..\tools\FEMSolver.Schur
EXE=$(EXE_PREFIX).exe


all: $(EXE)

clean:
	del /Q *.obj $(EXE) $(EXE).manifest $(EXE_PREFIX).pdb

######################################################################
# FEMSolver ##########################################################
######################################################################
$(EXE): main.schur.obj
	$(LINK) /OUT:$@ $** $(LDFLAGS)

main.schur.obj: $(SRCDIR)\Tools\FEMSolver\main.schur.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) /Fo$@ /c $?
