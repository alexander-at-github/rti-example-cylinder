#pragma once

#include <rti/particle/i_particle_factory.hpp>

#include "particle.hpp"

template<typename numeric_type>
class particle_factory : public rti::particle::i_particle_factory<numeric_type> {
public:
  std::unique_ptr<rti::particle::i_particle<numeric_type> > create() override final
  {
    return std::make_unique<particle<numeric_type>>();
  }
};
