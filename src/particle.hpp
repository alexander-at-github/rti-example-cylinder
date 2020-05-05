#pragma once

#include <rti/particle/i_particle.hpp>

template<typename numeric_type>
class particle : public rti::particle::i_particle<numeric_type> {
public:
  numeric_type process_hit(size_t primID, std::array<numeric_type, 3> direction) override final
  {
    return 0.1;
  }

  void init_new() override final
  {
    return;
  }
};
