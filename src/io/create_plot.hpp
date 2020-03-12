#pragma once

#include <algorithm>
#include <iterator>
#include <tuple>
#include <vector>

#include "gnuplot-iostream.h"

namespace io {

  template<typename numeric_type, typename numeric_type_o>
  static std::vector<numeric_type>
  create_buckets(std::vector<std::array<numeric_type, 3> > points, std::vector<numeric_type> mcestimates)
  {
    assert (points.size() == mcestimates.size() && "Precondition");
    auto numOfBuckets = 128;
    auto buckets = std::vector<numeric_type_o> (numOfBuckets, 0); // number of elements and initial value
    auto zvalToMCEstMap = std::vector<std::pair<numeric_type, numeric_type> > {};
    for (size_t idx = 0; idx < points.size(); ++idx) {
      auto pointzval = points[idx][2];
      auto pointMCEst = mcestimates[idx];
      zvalToMCEstMap.push_back({pointzval, pointMCEst});
    }
    // sort vector by first element of pair
    std::sort(zvalToMCEstMap.begin(), zvalToMCEstMap.end(),
      [](auto const& p1, auto const& p2) { return p1.first < p2.first; });

    auto zDelta = std::fabs((double) zvalToMCEstMap.front().first - zvalToMCEstMap.back().first) / numOfBuckets;
    auto zmin = (double) zvalToMCEstMap.front().first;

    // std::cout
    //   << "zmin == " << zmin << std::endl
    //   << "zmax == " << zvalToMCEstMap.back().first << std::endl
    //   << "zDelta == " << zDelta << std::endl;

    auto bucketidx = 0u;
    auto pointsperbucket = std::vector<size_t> (buckets.size(), 0);
    for (auto const& pair : zvalToMCEstMap) {
      auto zcoord = pair.first;
      auto mcvalue = pair.second;

      assert(zmin <= zcoord && "Error");
      while (zmin + zDelta < zcoord) {
        // move to next bucket
        // std::cout << "###" << std::endl;
        // std::cout << "zmin == " << zmin << std::endl;
        // std::cout << "zmin + zDelta == " << zmin + zDelta << std::endl;
        // std::cout << "bucketidx == " << bucketidx << std::endl;
        // std::cout << "zcoord == " << zcoord << std::endl;
        bucketidx += 1;
        assert(bucketidx < buckets.size() && "Correctness Assertion");
        buckets[bucketidx] = 0;
        zmin += zDelta;
      }
      // std::cout << "bucketidx == " << bucketidx << std::endl;
      assert (zmin <= zcoord && zcoord <= zmin + zDelta && "Correctness Assertion");
      assert (bucketidx < buckets.size() && "Correctness Assertion");
      buckets[bucketidx] += mcvalue;
      pointsperbucket[bucketidx] += 1;
    }
    assert (bucketidx < buckets.size() && "Correctness Assertion");
    for (size_t idx = 0; idx < buckets.size(); ++idx) {
      buckets[idx] /= pointsperbucket[idx];
    }
    return buckets;
  }

  template<typename numeric_type>
  static void
  plot(std::vector<numeric_type> buckets) {
    auto gpstr = std::stringstream {};
    gpstr
      << "set title 'Flux in cylinder' \n"
      << "plot '-' with lines \n";

    for (auto const& value : buckets)
      gpstr << value << "\n";
    Gnuplot gnup {};
    gnup << gpstr.str();
  }

  template<typename numeric_type>
  static void
  create_plot(std::vector<std::array<numeric_type, 3> > points, std::vector<numeric_type> mcestimates)
  {
    assert (points.size() == mcestimates.size() && "Precondition");
    auto buckets = create_buckets<numeric_type, numeric_type>(points, mcestimates);
    plot(buckets);
  }

}
