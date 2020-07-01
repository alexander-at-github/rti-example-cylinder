#pragma once

#include <vtkAbstractArray.h>
#include <vtkCellIterator.h>
#include <vtkCellData.h>
#include <vtkDataSetAttributes.h>
#include <vtkGenericCell.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtksys/SystemTools.hxx>
#include <vtkXMLPolyDataReader.h>

namespace io {

  template<typename numeric_type>
  bool is_normalized(std::array<numeric_type, 3> const& pV)
  {
    auto epsilon = 1e-6f;
    numeric_type length = std::sqrt(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2]);
    return 1-epsilon <= length && length <= 1+epsilon;
  }

  // This function modifies its argument when called
  template<typename numeric_type>
  void normalize(std::array<numeric_type, 3>& pV)
  {
    numeric_type thrdNorm = std::sqrt(pV[0] * pV[0] + pV[1] * pV[1] + pV[2] * pV[2]);
    pV[0] /= thrdNorm;
    pV[1] /= thrdNorm;
    pV[2] /= thrdNorm;
    if ( ! is_normalized(pV) )
      std::cerr << "Warning: Assertion error is about to happen. thrdNorm == " << thrdNorm << std::endl;
    assert( is_normalized(pV) && "Postcondition" );
  }

  template<typename numeric_type>
  class vtp_point_cloud_reader {
  public:
    vtp_point_cloud_reader(const std::string& pFilename) :
      mInfilename(pFilename) {
      std::string extension = vtksys::SystemTools::GetFilenameLastExtension(pFilename);
      if (extension != ".vtp") {
        std::cerr
          << "Warning: " << typeid(this).name()
          << " may not be able to read the file " << pFilename
          << " because it does not have the .vtp extension." << std::endl;
      }
      auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
      reader->SetFileName(pFilename.c_str());
      reader->Update();
      vtkSmartPointer<vtkPolyData> polydata = reader->GetOutput();
      vtkIdType numPnts = polydata->GetNumberOfPoints();
      vtkIdType numCells = polydata->GetNumberOfCells();
      if (numPnts != numCells) {
        std::cerr
          << "Warning: " << typeid(this).name()
          << "expects an input such that number of points equals number of cells. "
          << "File: " << pFilename << std::endl;
      }
      vtkSmartPointer<vtkCellData> celldata = polydata->GetCellData();
      std::string sqrtOfAreaStr = "sqrt-of-area";
      //std::string sqrtOfAreaStr = "Points_Magnitude";
      celldata->SetActiveAttribute(sqrtOfAreaStr.c_str(), vtkDataSetAttributes::SCALARS);
      vtkSmartPointer<vtkDataArray> sqrtOfAreaArray = celldata->GetScalars();
      if (sqrtOfAreaArray == nullptr) {
        std::cerr
          << "Warning: " << typeid(this).name()
          << " could not find cell data with the name \"" << sqrtOfAreaStr << "\" in the file "
          << pFilename << std::endl;
      }
      vtkSmartPointer<vtkDataArray> normals = celldata->GetNormals();
      if (normals == nullptr) {
        std::cerr
          << "Warning: " << typeid(this).name()
          << " could not find surface normal data in the file " << pFilename << std::endl;
      }
      for (vtkIdType idx = 0; idx < numPnts; ++idx) {
        double xyz[3]; // 3 dimensions
        polydata->GetPoint(idx, xyz);
        double radius[1]; // 1 dimension
        sqrtOfAreaArray->GetTuple(idx, radius);
        double radiusEpsilon = 1.0 / 32; // about 3%
        //radius[0] *= (1 + radiusEpsilon); // std::sqrt(3.0)/2 * (1 + radiusEpsilon);
        // radius[0] *= (1 + radiusEpsilon);
        auto point =
          std::array<numeric_type, 3> {(numeric_type) xyz[0], (numeric_type) xyz[1] , (numeric_type) xyz[2]};
        mPoints.push_back(point);
        mRadii.push_back((numeric_type) radius[0]);
        double nxnynz[3];
        normals->GetTuple(idx, nxnynz);
        auto normal =
          std::array<numeric_type, 3> {(numeric_type) nxnynz[0], (numeric_type) nxnynz[1], (numeric_type) nxnynz[2]};
        // Normalize
        if ( ! is_normalized(normal))
          normalize(normal);
        mNormals.push_back(normal);
      }
      mPoints.shrink_to_fit();
      mRadii.shrink_to_fit();
      mNormals.shrink_to_fit();
    }

    ~vtp_point_cloud_reader() {}

    std::vector<std::array<numeric_type, 3> > get_points()
    {
      return mPoints;
    }

    std::vector<numeric_type> get_radii()
    {
      return mRadii;
    }

    std::vector<std::array<numeric_type, 3> > get_normals()
    {
      return mNormals;
    }

    std::string get_input_file_name()
    {
      return mInfilename;
    }

  private:
    std::string mInfilename;
    std::vector<std::array<numeric_type, 3> > mPoints;
    std::vector<std::array<numeric_type, 3> > mNormals;
    std::vector<numeric_type> mRadii;
  };
}
