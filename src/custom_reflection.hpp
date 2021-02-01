#pragma once

#include "rti/reflection/i_reflection.hpp"

template<typename numeric_type>
class custom_reflection : public rti::reflection::i_reflection<numeric_type> {

public:

  rti::util::pair<rti::util::triple<numeric_type> >
  use
  (RTCRay& rayin, RTCHit& hitin, rti::geo::meta_geometry<numeric_type>& geometry,
   rti::rng::i_rng& rng, rti::rng::i_rng::i_state& rngstate)
  {
    // Use `rng.get(rngstate)` to aquire a random number.
    // This way of drawing random numbers is compatible with RTI's parallel execution
    // and Monte Carlo simulation.
    auto rndm = ((double) rng.get(rngstate)) / rng.max(); // random in [0,1]

    // Incoming ray direction
    auto indir = std::array<numeric_type, 3> {rayin.dir_x, rayin.dir_y, rayin.dir_z};
    rti::util::inv(indir);
    // Surface normal
    auto normal = geometry.get_normal(hitin.primID);
    auto cosphi = rti::util::dot_product(indir, normal) / rti::util::length_of_vec(indir) / rti::util::length_of_vec(normal);

    if (cosphi < thrshld || cosphi < rndm) {
      return diffuse.use(rayin, hitin, geometry, rng, rngstate);
    }
    return specular.use(rayin, hitin, geometry, rng, rngstate);
  }

private:

  numeric_type thrshld = 0.5;
  rti::reflection::diffuse<numeric_type> diffuse;
  rti::reflection::specular<numeric_type> specular;
};
