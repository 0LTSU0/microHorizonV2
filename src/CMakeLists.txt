# others
add_subdirectory(Tracer)

# "workers"
add_subdirectory(shared)
add_subdirectory(posInputWorker)
add_subdirectory(osmLoadWorker)
add_subdirectory(horizonWorker)
add_subdirectory(fancyFrontEnd)

# main executable
add_subdirectory(microHorizonApp)