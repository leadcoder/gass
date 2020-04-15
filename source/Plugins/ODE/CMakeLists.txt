find_package(ODE REQUIRED)
gass_setup_plugin(GASSPluginODE PRIVATE_DEPS ode::ode)