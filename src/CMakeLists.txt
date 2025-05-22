# others
add_subdirectory(Tracer)

# "workers"
add_subdirectory(shared)
add_subdirectory(posInputWorker)
add_subdirectory(osmLoadWorker)
add_subdirectory(horizonWorker)

# main executable
add_subdirectory(microHorizonApp)