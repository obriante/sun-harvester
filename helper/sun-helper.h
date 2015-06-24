/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Universita' Mediterranea di Reggio Calabria (UNIRC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Giovanni Benigno <giovanni.benigno.954@studenti.unirc.it>
 * Orazio Briante <orazio.briante@unirc.it>
 */



#ifndef SUN_HELPER_H
#define SUN_HELPER_H

#include <ns3/object-factory.h>
#include <ns3/ptr.h>
#include <ns3/sun.h>
#include <string>

namespace ns3 {

/**
 * \ingroup energy
 * \brief Creates a Sun  object.
 */
class SunHelper
{
public:
  SunHelper ();
  ~SunHelper ();

  void Set (std::string name, const AttributeValue &v);
  Ptr<Sun> Install ();
private:
  ObjectFactory m_Sun;

};

} // namespace ns3

#endif /* defined(SUN_HELPER_H) */
