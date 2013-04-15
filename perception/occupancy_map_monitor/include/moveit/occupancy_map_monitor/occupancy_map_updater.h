/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Jon Binney, Ioan Sucan */

#ifndef MOVEIT_OCCUPANCY_MAP_MONITOR_OCCUPANCY_MAP_UPDATER_
#define MOVEIT_OCCUPANCY_MAP_MONITOR_OCCUPANCY_MAP_UPDATER_

#include <moveit/occupancy_map_monitor/occupancy_map.h>
#include <moveit/mesh_filter/mesh_filter_base.h>
#include <geometric_shapes/shapes.h>
#include <boost/shared_ptr.hpp>

namespace occupancy_map_monitor
{

// The type for the shape handle should be the same as the type of the mesh handle
typedef mesh_filter::MeshHandle ShapeHandle;
typedef std::map<ShapeHandle, Eigen::Affine3d, std::less<ShapeHandle>, 
                 Eigen::aligned_allocator<std::pair<const ShapeHandle, Eigen::Affine3d> > > ShapeTransformCache;
typedef boost::function<bool(const std::string &target_frame, const ros::Time &target_time, ShapeTransformCache &cache)> TransformCacheProvider;

class OccupancyMapMonitor;

/** \brief Base class for classes which update the occupancy map.
 */
class OccupancyMapUpdater
{
public:

  OccupancyMapUpdater(OccupancyMapMonitor *monitor, const std::string &type);  
  virtual ~OccupancyMapUpdater();


  /** @brief Set updater params using struct that comes from parsing a yaml string*/
  virtual bool setParams(XmlRpc::XmlRpcValue &params) = 0;

  /** @brief Do any necessary setup (subscribe to ros topics, etc.)*/
  virtual bool initialize() = 0;

  virtual void start() = 0;
  
  virtual void stop() = 0;

  virtual mesh_filter::MeshHandle excludeShape(const shapes::ShapeConstPtr &shape) = 0;
  
  virtual void forgetShape(mesh_filter::MeshHandle handle) = 0;
  
  const std::string& getType() const
  {
    return type_;
  } 
  
  void setTransformCacheCallback(const TransformCacheProvider &transform_callback)
  {
    transform_provider_callback_ = transform_callback;
  }
  
  void publishDebugInformation(bool flag)
  {
    debug_info_ = flag;
  }
  
protected:
  
  OccupancyMapMonitor *monitor_; 
  std::string type_;  
  OccMapTreePtr tree_;
  TransformCacheProvider transform_provider_callback_;
  ShapeTransformCache transform_cache_;
  bool debug_info_;
  
  bool updateTransformCache(const std::string &target_frame, const ros::Time &target_time)
  {
    transform_cache_.clear();
    return transform_provider_callback_ ? transform_provider_callback_(target_frame, target_time, transform_cache_) : false;
  }
  
};

typedef boost::shared_ptr<OccupancyMapUpdater> OccupancyMapUpdaterPtr;
typedef boost::shared_ptr<const OccupancyMapUpdater> OccupancyMapUpdaterConstPtr;

}

#endif /* MOVEIT_OCCUPANCY_MAP_UPDATER_H_ */
