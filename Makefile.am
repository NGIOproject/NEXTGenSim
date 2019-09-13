#
# Copyright 2007 Francesc Guim Bernat & Barcelona Supercomputing Centre
# Copyright 2015-2019 NEXTGenIO Project [EC H2020 Project ID: 671951] (www.nextgenio.eu)
# Copyright 2019 Daniel Rivas & Barcelona Supercomputing Centre
#
# This file is part of NEXTGenSim.
# 
# NEXTGenSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# NEXTGenSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEXTGenSim.  If not, see <https://www.gnu.org/licenses/>.
#


bin_PROGRAMS=nextgensim

nextgensim_CXXFLAGS=-Wall -g
nextgensim_CPPFLAGS=-I/usr/include -I$(srcdir)/src -I/usr/include/libxml2
nextgensim_LDFLAGS=-L/usr/lib
nextgensim_LDADD=-lm -lboost_system -lboost_filesystem -lboost_iostreams -lxml2
AUTOMAKE_OPTIONS = foreign subdir-objects


nextgensim_SOURCES = \
src/nextgensim.cpp \
src/utils/simulationconfiguration.cpp \
src/utils/paraverentry.cpp \
src/utils/otf2trace.cpp \
src/utils/utilities.cpp \
src/utils/fcfsjobqueue.cpp \
src/utils/rscript.cpp \
src/utils/arg_parser.cpp \
src/utils/estimatefinishjobqueue.cpp \
src/utils/log.cpp\
src/utils/jobqueue.cpp\
src/utils/lxwfjobqueue.cpp\
src/utils/csvpolicyinfoconverter.cpp\
src/utils/cvsjobsinfoconverter.cpp\
src/utils/cvsconverter.cpp\
src/utils/configurationfile.cpp\
src/utils/predictorconfigurationfile.cpp\
src/utils/architectureconfiguration.cpp\
src/utils/jobcsvrscriptanaliser.cpp\
src/utils/estimatedruntimejobqueue.cpp\
src/utils/realfinishjobqueue.cpp\
src/utils/executecommand.cpp\
src/utils/matrix.cpp\
src/utils/paravertrace.cpp\
src/utils/ruajobqueue.cpp\
src/utils/starttimejobqueue.cpp\
src/utils/cvsstreamer.cpp\
src/scheduling/swftraceextended.cpp\
src/scheduling/reservationtable.cpp\
src/scheduling/simulatorcharacteristics.cpp\
src/scheduling/metric.cpp\
src/scheduling/fcfsschedulingpolicy.cpp\
src/scheduling/powerconsumption.cpp\
src/scheduling/policyentitycollector.cpp\
src/scheduling/simulation.cpp\
src/scheduling/swftracefile.cpp\
src/scheduling/schedulingpolicy.cpp\
src/scheduling/resourceaggregatedinfo.cpp\
src/scheduling/tracefile.cpp\
src/scheduling/resourceusage.cpp\
src/scheduling/jobrequirement.cpp\
src/scheduling/simulationevent.cpp\
src/scheduling/joballocation.cpp\
src/scheduling/job.cpp\
src/scheduling/relationshipaggregatedinfo.cpp\
src/statistics/process.cpp\
src/statistics/statistics.cpp\
src/statistics/processnormal.cpp\
src/statistics/processpoisson.cpp\
src/statistics/processerlang.cpp\
src/statistics/statisticalestimator.cpp\
src/statistics/processtriangular.cpp\
src/statistics/average.cpp\
src/statistics/processexponential.cpp\
src/statistics/processuniform.cpp\
src/statistics/standarddeviation.cpp\
src/statistics/min.cpp\
src/statistics/processconstant.cpp\
src/statistics/percentile95.cpp\
src/statistics/statisticsconfigurationfile.cpp\
src/statistics/countliterals.cpp\
src/statistics/processweibull.cpp\
src/statistics/accumulateliterals.cpp\
src/statistics/accumulate.cpp\
src/statistics/percentiles.cpp\
src/statistics/processlognormal.cpp\
src/statistics/max.cpp\
src/statistics/interquartiledifference.cpp\
src/statistics/median.cpp\
src/statistics/processgeometric.cpp\
src/archdatamodel/node.cpp\
src/archdatamodel/partitionlist.cpp\
src/archdatamodel/hardware.cpp\
src/archdatamodel/resource.cpp\
src/archdatamodel/compute.cpp\
src/scheduling/partition.cpp\
src/archdatamodel/storage.cpp\
src/archdatamodel/memory.cpp\
src/scheduling/systemorchestrator.cpp\
src/scheduling/resourcebucket.cpp\
src/scheduling/resourcereservationtable.cpp\
src/scheduling/resourcejoballocation.cpp\
src/utils/jobqueue.h\
src/utils/paravertrace.h\
src/utils/otf2trace.h\
src/utils/realfinishjobqueue.h\
src/utils/cvsstreamer.h\
src/utils/cvsconverter.h\
src/utils/executecommand.h\
src/utils/csvpolicyinfoconverter.h\
src/utils/predictorconfigurationfile.h\
src/utils/estimatedruntimejobqueue.h\
src/utils/simulationconfiguration.h\
src/utils/rscript.h\
src/utils/utilities.h\
src/utils/arg_parser.h\
src/utils/lxwfjobqueue.h\
src/utils/configurationfile.h\
src/utils/jobcsvrscriptanaliser.h\
src/utils/fcfsjobqueue.h\
src/utils/log.h\
src/utils/starttimejobqueue.h\
src/utils/ruajobqueue.h\
src/utils/estimatefinishjobqueue.h\
src/utils/cvsjobsinfoconverter.h\
src/utils/architectureconfiguration.h\
src/utils/matrix.h\
src/utils/paraverentry.h\
src/scheduling/powerconsumption.h\
src/scheduling/resourceaggregatedinfo.h\
src/scheduling/relationshipaggregatedinfo.h\
src/scheduling/resourceusage.h\
src/scheduling/schedulingpolicy.h\
src/scheduling/metric.h\
src/scheduling/policyentitycollector.h\
src/scheduling/reservationtable.h\
src/scheduling/swftracefile.h\
src/scheduling/tracefile.h\
src/scheduling/joballocation.h\
src/scheduling/job.h\
src/scheduling/swftraceextended.h\
src/scheduling/simulationevent.h\
src/scheduling/jobrequirement.h\
src/scheduling/simulation.h\
src/scheduling/simulatorcharacteristics.h\
src/scheduling/fcfsschedulingpolicy.h\
src/statistics/processlognormal.h\
src/statistics/processgeometric.h\
src/statistics/processconstant.h\
src/statistics/processtriangular.h\
src/statistics/interquartiledifference.h\
src/statistics/accumulateliterals.h\
src/statistics/processpoisson.h\
src/statistics/percentile95.h\
src/statistics/statisticsconfigurationfile.h\
src/statistics/percentiles.h\
src/statistics/processerlang.h\
src/statistics/statisticalestimator.h\
src/statistics/process.h\
src/statistics/max.h\
src/statistics/processweibull.h\
src/statistics/countliterals.h\
src/statistics/processnormal.h\
src/statistics/processuniform.h\
src/statistics/median.h\
src/statistics/standarddeviation.h\
src/statistics/accumulate.h\
src/statistics/average.h\
src/statistics/processexponential.h\
src/statistics/min.h\
src/statistics/statistics.h\
src/archdatamodel/node.h\
src/archdatamodel/rank.h\
src/archdatamodel/hardware.h\
src/archdatamodel/resource.h\
src/archdatamodel/storage.h\
src/archdatamodel/memory.h\
src/archdatamodel/compute.h\
src/scheduling/systemorchestrator.h\
src/scheduling/resourcebucket.h\
src/scheduling/resourcereservationtable.h\
src/scheduling/resourcejoballocation.h\
src/scheduling/partition.h\
src/utils/trace.cpp\
src/utils/trace.h\
src/scheduling/fcfsschedulingpolicy.h\
src/utils/pbsInput.cpp\
src/utils/pbsInput.h\
src/utils/nntfInput.cpp\
src/utils/nntfInput.h\
src/scheduling/slurmschedulingpolicy.cpp\
src/scheduling/slurmschedulingpolicy.h\
src/utils/slurmjobqueue.cpp\
src/utisl/slurmjobqueue.h\
$(END)


