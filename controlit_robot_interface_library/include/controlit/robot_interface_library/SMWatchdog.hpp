/*
 * Copyright (C) 2015 The University of Texas at Austin and the
 * Institute of Human Machine Cognition. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 2.1 of
 * the License, or (at your option) any later version. See
 * <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 */

#ifndef __CONTROLIT_ROBOT_INTERFACE_LIBRARY_SM_WATCHDOG_HPP__
#define __CONTROLIT_ROBOT_INTERFACE_LIBRARY_SM_WATCHDOG_HPP__

#include <ros/ros.h>
#include "SharedMemoryControlInterface.hpp"
#include <signal.h>

namespace controlit {
namespace robot_interface_library {

class SMWatchdog
{
public:
    SMWatchdog(const ros::NodeHandle& nh);
    ~SMWatchdog();
    void spin();

private:
    ros::NodeHandle m_nh;
    double m_loop_rate;
};

} // namespace robot_interface_library
} // namespace controlit

#endif // __CONTROLIT_ROBOT_INTERFACE_LIBRARY_SM_WATCHDOG_HPP__
