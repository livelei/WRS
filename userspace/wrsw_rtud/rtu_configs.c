/*
 * White Rabbit RTU (Routing Table Unit)
 * Copyright (C) 2013, CERN.
 *
 * Version:     wrsw_rtud v2.0-dev
 *
 * Authors:     Maciej Lipinski (maciej.lipinski@cern.ch)
 *              
 *
 * Description: RTU initial configuration
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rtu_configs.h"
#include "rtu_tatsu_drv.h"
#include "rtu_ep_drv.h"
#include "rtu_ext_drv.h"

uint8_t mac_single_spirent_A[] = {0x00,0x10,0x94,0x00,0x00,0x01}; // spirent MAC of port 1
uint8_t mac_single_spirent_B[] = {0x00,0x10,0x94,0x00,0x00,0x02}; // spirent MAC of port 2
uint8_t mac_single_spirent_C[] = {0x00,0x10,0x94,0x00,0x00,0x03}; // spirent MAC of port 1
uint8_t mac_single_spirent_D[] = {0x00,0x10,0x94,0x00,0x00,0x04}; // spirent MAC of port 2  

uint8_t mac_single_PC_ETH6[]   = {0x90,0xe2,0xba,0x17,0xa7,0xAF}; // test PC
uint8_t mac_single_PC_ETH7[]   = {0x90,0xe2,0xba,0x17,0xa7,0xAC}; // test PC 90:e2:ba:17:a7:AC
uint8_t mac_single_PC_ETH8[]   = {0x90,0xe2,0xba,0x17,0xa7,0xAE}; // test PC
uint8_t mac_single_PC_ETH9[]   = {0x90,0xe2,0xba,0x17,0xa7,0xAD}; // test PC

uint8_t bcast_mac[]            = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/**
 * opt=0
 */
int config_info()
{
  
  TRACE(TRACE_INFO, "Choice of predefined startup configurations of our super features");
  TRACE(TRACE_INFO, "Usage: wrsw_rtu -s [<option> <value>]");
  TRACE(TRACE_INFO, "-s 0         show this info");
  TRACE(TRACE_INFO, "-s 1         Default: TRU transparent, no RTUex features");
  TRACE(TRACE_INFO, "-s 2         Config like for TRU switch-over demo (but no rtu_thread")
  TRACE(TRACE_INFO, "             running): port 1 active, port 2 backup for p1, ports 6 ");
  TRACE(TRACE_INFO, "             and 7 enabled");
  TRACE(TRACE_INFO, "-s 3         Snake test for >normal traffic< : separate VLAN for each ");
  TRACE(TRACE_INFO, "             pair of ports (starting with VID=1 @ port 0), tagging on ");
  TRACE(TRACE_INFO, "             ingress, untagging on egress");
  TRACE(TRACE_INFO, "-s 4  n      Snake test for >fast forward traffic< : separate VLAN for ");
  TRACE(TRACE_INFO, "             each pair of ports (starting with VID=1 @ port 0), tagging ");
  TRACE(TRACE_INFO, "             on ingress, untagging on egress");
  TRACE(TRACE_INFO, "             0: single MAC entry for Spirent");
  TRACE(TRACE_INFO, "             1: single MAC entry for Test PC");
  TRACE(TRACE_INFO, "             2: range  MAC entry for Spirent");
  TRACE(TRACE_INFO, "             3: range  MAC entry for Test PC");
  TRACE(TRACE_INFO, "             4: Broadcast");
  TRACE(TRACE_INFO, "-s 5  n      Test VLANS:");
  TRACE(TRACE_INFO, "             0: VID/FID=1 with mask 0xF and static entry for broadcast to all ports");
  TRACE(TRACE_INFO, "             1: VIDs/FIDs={1-18} with mask 0x1<<i and broadcast for all ports");
  TRACE(TRACE_INFO, "             2: VID/FID=0 with mask 0xF and broadcast for all ports");
  TRACE(TRACE_INFO, "             3: VIDs/FIDs={1-18} with mask 0x3<<i and broadcast for all ports");
  TRACE(TRACE_INFO, "             4: VIDs={0-1}, FID=0 with mask 0xF<<i and broadcast for all ports");  
  TRACE(TRACE_INFO, "-s 6  n      Snake test for >fast forward traffic< (hacked): separate VLAN for ");
  TRACE(TRACE_INFO, "             each pair of ports (starting with VID=0 @ port 0), tagging ");
  TRACE(TRACE_INFO, "             on ingress, untagging on egress");
  TRACE(TRACE_INFO, "             0: single MAC entry for Spirent");
  TRACE(TRACE_INFO, "             1: single MAC entry for Test PC");
  TRACE(TRACE_INFO, "             2: range  MAC entry for Spirent");
  TRACE(TRACE_INFO, "             3: range  MAC entry for Test PC");
  TRACE(TRACE_INFO, "             4: Broadcast");
  TRACE(TRACE_INFO, "             5: nothing special");
  TRACE(TRACE_INFO, "-s 7  n      High Prio tunneling, kind of:  ");
  TRACE(TRACE_INFO, "      0      nonHP frame dropping disabled ");    
  TRACE(TRACE_INFO, "      1      nonHP frame dropping enabled ");      
  return 0;
}

/**
 * opt=1
 */
int config_default(int sub_opt, int port_num)
{
  int i;
//   tru_enable(); // should be transparent -> bug ????????
   tru_disable();
   for(i=0;i<port_num;i++)
     ep_set_vlan((uint32_t)i, 0x2/*qmode*/, 0 /*fix_prio*/, 0 /*prio_val*/, 0 /*pvid*/);
  return 0;
}

/**
 * opt=2
 */
int config_tru_demo_test(int sub_opt, int port_num)
{
  int i;
  
  for(i=0;i<port_num;i++)
     ep_set_vlan((uint32_t)i, 0x2/*qmode*/, 0 /*fix_prio*/, 0 /*prio_val*/, 0 /*pvid*/);  
  tru_set_port_roles(1 /*active port*/,2/*backup port*/, 0 /*fid*/); //TODO: make it config
  tru_enable(); // should be transparent
  return 0;
}

/**
 * opt=3
 */
int config_snake_standard_traffic_test(int sub_opt, int port_num)
{
  int i;
  int pvid = 1;
  tru_disable();
  // for snake test
  for(i=0;i < 18;i++)
  {
    if(i%2==0 && i!=0) pvid++;
    vlan_entry_vd( pvid,          //vid, 
                  (0x3 << 2*(pvid-1)),  //port_mask, 
                  pvid,      //fid, 
                  0,            //prio,
                  0,            //has_prio,
                  0,            //prio_override, 
                  0             //drop
                  );     
          
   }    
   ep_snake_config(1 /*VLANS 0-17 port, access/untag*/);   
   return 0;  
}
/**
 * opt = 4
 * 
 * sub_opt:
 * 0: single mac entry test (Spirent)
 * 1: single mac entry test (PC)
 * 2: range mac entry test (Spirent)
 * 3: range mac entry test (PC)
 */
   
int config_snake_ff_traffic_test(int sub_opt, int port_num)
{
  int i;
  int pvid = 1;       
  tru_disable();
  
  //broadcast for VLAN = 0
  
  rtu_fd_create_entry(bcast_mac, 0, 0x3, STATIC, OVERRIDE_EXISTING); 
  // for snake test
  for(i=0;i < 18;i++)
  {
     if(i%2==0 && i!=0) pvid++;
     vlan_entry_vd( pvid,          //vid, 
                   (0x3 << 2*(pvid-1)),  //port_mask, 
                    pvid,      //fid, 
                    0,            //prio,
                    0,            //has_prio,
                    0,            //prio_override, 
                    0             //drop
                 );     
     rtu_fd_create_entry(bcast_mac, pvid, (0x3 << 2*(pvid-1)), STATIC, OVERRIDE_EXISTING);    
  }    
  switch(sub_opt)
  {
    case 0:
      rtux_add_ff_mac_single(0/*ID*/, 1/*valid*/, mac_single_spirent_A/*MAC*/);
      rtux_add_ff_mac_single(1/*ID*/, 1/*valid*/, mac_single_spirent_B/*MAC*/);
      rtux_add_ff_mac_single(2/*ID*/, 1/*valid*/, mac_single_spirent_C/*MAC*/);
      rtux_add_ff_mac_single(3/*ID*/, 1/*valid*/, mac_single_spirent_D/*MAC*/);
      break;
    case 1:
      rtux_add_ff_mac_single(0/*ID*/, 1/*valid*/, mac_single_PC_ETH6/*MAC*/);
      rtux_add_ff_mac_single(1/*ID*/, 1/*valid*/, mac_single_PC_ETH7/*MAC*/);
      rtux_add_ff_mac_single(2/*ID*/, 1/*valid*/, mac_single_PC_ETH8/*MAC*/);
      rtux_add_ff_mac_single(3/*ID*/, 1/*valid*/, mac_single_PC_ETH9/*MAC*/);     
      break;
    case 2:
      rtux_add_ff_mac_range (0/*ID*/, 1/*valid*/, mac_single_spirent_A/*MAC_lower*/, 
                                                  mac_single_spirent_D /*MAC_upper*/);        
    break;
    case 3:
      rtux_add_ff_mac_range (0/*ID*/, 1/*valid*/, mac_single_PC_ETH7/*MAC_lower*/, 
                                                  mac_single_PC_ETH6/*MAC_upper*/);             
      break;
    case 4:
      // nothing to do for broadcast
      break;
    default:
      TRACE(TRACE_INFO,"config_ff_in_vlan_test: wrong sub_opt  %d ",sub_opt);
      return -1;
    break;
  }
      
  if(sub_opt == 0 || sub_opt == 1)
    rtux_feature_ctrl(0 /*mr*/, 
                      0 /*mac_ptp*/, 
                      0/*mac_ll*/, 
                      1/*mac_single*/, 
                      0/*mac_range*/, 
                      0/*mac_br*/,
                      0/*drop when full_match full*/);
  else if(sub_opt == 2 || sub_opt == 3)
    rtux_feature_ctrl(0 /*mr*/, 
                      0 /*mac_ptp*/, 
                      0/*mac_ll*/, 
                      0/*mac_single*/, 
                      1/*mac_range*/, 
                      0/*mac_br*/,
                      0/*drop when full_match full*/);
  else if(sub_opt == 4)                     
    rtux_feature_ctrl(0 /*mr*/, 
                      0 /*mac_ptp*/, 
                      0/*mac_ll*/, 
                      0/*mac_single*/, 
                      0/*mac_range*/, 
                      1/*mac_br*/,
                      0/*drop when full_match full*/);
    
  ep_snake_config(1 /*VLANS 0-17 port, access/untag*/);   
  return 0;
}

/**
 * opt=5
 */
int config_VLAN_test(int sub_opt, int port_num)
{
  int i, pvid=0;

  tru_disable();
  if(sub_opt == 0)
  {
    vlan_entry_vd( 1,            //vid, 
                   0xF,          //port_mask, 
                   1,            //fid, 
                   0,            //prio,
                   0,            //has_prio,
                   0,            //prio_override, 
                   0             //drop
                 );  
    rtu_fd_create_entry(bcast_mac, 1, 0xFFFFFFFF , STATIC, OVERRIDE_EXISTING);
  }
  else if(sub_opt == 1)
  {
    for(i=0;i < 18;i++)
    {
      
      vlan_entry_vd(pvid,           //vid, 
                    (0x1 << pvid),  //port_mask, 
                    pvid,           //fid, 
                    0,              //prio,
                    0,              //has_prio,
                    0,              //prio_override, 
                    0               //drop
                    );     
      rtu_fd_create_entry(bcast_mac, pvid, 0xFFFFFFFF , STATIC, OVERRIDE_EXISTING);
      pvid++; 
    }        
  }
  else if(sub_opt == 2)
  {
    vlan_entry_vd( 0,            //vid, 
                   0xF,          //port_mask, 
                   0,            //fid, 
                   0,            //prio,
                   0,            //has_prio,
                   0,            //prio_override, 
                   0             //drop
                 );  
  }
  else if(sub_opt == 3)
  {
    pvid = 1;
    for(i=0;i < 18;i++)
    {
      vlan_entry_vd( pvid,          //vid, 
                    (0x3 << (pvid-1)),  //port_mask, 
                     pvid,      //fid, 
                     0,            //prio,
                     0,            //has_prio,
                     0,            //prio_override, 
                     0             //drop
                    );     
       rtu_fd_create_entry(bcast_mac, pvid, 0xFFFFFFFF , STATIC, OVERRIDE_EXISTING);
       pvid++; 
    } 
  }
  else if(sub_opt == 4)
  {
    /* this case does not work for both: Fast and Full match(es)
     * 1. for full match for VID=1 it drops
     *
     */
    vlan_entry_vd( 0,            //vid, 
                   0x7,          //port_mask, 
                   0,            //fid, 
                   0,            //prio,
                   0,            //has_prio,
                   0,            //prio_override, 
                   0             //drop
                 ); 
    vlan_entry_vd( 1,            //vid, 
                   0xF8,          //port_mask, 
                   0,            //fid, 
                   0,            //prio,
                   0,            //has_prio,
                   0,            //prio_override, 
                   0             //drop
                 ); 
    rtu_fd_create_entry(bcast_mac, 0, 0xFFFFFFFF , STATIC, OVERRIDE_EXISTING);    
  }  
  else
  { 
    TRACE(TRACE_INFO,"config_VLAN_test: wrong sub_opt  %d ",sub_opt);
    return -1;   
  }
   for(i=0;i<port_num;i++)
     ep_set_vlan((uint32_t)i, 0x2/*qmode*/, 0 /*fix_prio*/, 0 /*prio_val*/, 0 /*pvid*/);
  return 0;
}
/**
 * opt=6
 */
int config_snake_ff_hacked_traffic_test(int sub_opt, int port_num)
{
  int i;
  int pvid = 1;
  tru_disable();
  // for snake test
  for(i=0;i < 18;i++)
  {
    if(i%2==0 && i!=0) pvid++;
    vlan_entry_vd( pvid,          //vid, 
                  (0x3 << 2*(pvid-1)),  //port_mask, 
                  pvid,      //fid, 
                  0,            //prio,
                  0,            //has_prio,
                  0,            //prio_override, 
                  0             //drop
                  );     
    // static entries for snake forwarding
//     rtu_fd_create_entry(bcast_mac, pvid, (0x1 << 2*(pvid))  , mac_single_PC_ETH9, ADD_TO_EXISTING);        
//     rtu_fd_create_entry(bcast_mac, pvid, (0x1 << 2*(pvid-1)), mac_single_PC_ETH8, ADD_TO_EXISTING);
  }    
  ep_snake_config(1 /*VLANS 0-17 port, access/untag*/); 
  
  vlan_entry_vd( 0,          //vid, 
                 0x3,  //port_mask, 
                 0,      //fid, 
                  0,            //prio,
                  0,            //has_prio,
                  0,            //prio_override, 
                  0             //drop
                  );    
 
      
  switch(sub_opt)
  {
    case 0:
//       rtux_add_ff_mac_single(0/*ID*/, 1/*valid*/, mac_single_spirent_A/*MAC*/);
//       rtux_add_ff_mac_single(1/*ID*/, 1/*valid*/, mac_single_spirent_B/*MAC*/);
      rtux_add_ff_mac_single(0/*ID*/, 1/*valid*/, mac_single_spirent_C/*MAC*/);
      rtux_add_ff_mac_single(1/*ID*/, 1/*valid*/, mac_single_spirent_D/*MAC*/);
      break;
    case 1:
      rtux_add_ff_mac_single(0/*ID*/, 1/*valid*/, mac_single_PC_ETH6/*MAC*/);
      rtux_add_ff_mac_single(1/*ID*/, 1/*valid*/, mac_single_PC_ETH7/*MAC*/);
      rtux_add_ff_mac_single(2/*ID*/, 1/*valid*/, mac_single_PC_ETH8/*MAC*/);
      rtux_add_ff_mac_single(3/*ID*/, 1/*valid*/, mac_single_PC_ETH9/*MAC*/);     
      break;
    case 2:
      rtux_add_ff_mac_range (0/*ID*/, 1/*valid*/, mac_single_spirent_A/*MAC_lower*/, 
                                                  mac_single_spirent_D /*MAC_upper*/);        
    break;
    case 3:
      rtux_add_ff_mac_range (0/*ID*/, 1/*valid*/, mac_single_PC_ETH7/*MAC_lower*/, 
                                                  mac_single_PC_ETH6/*MAC_upper*/);             
      break;
    case 4:
      // nothing to do for broadcast
    case 5:
      // nothing to do for "normal" snake
      break;
    default:
      TRACE(TRACE_INFO,"config_ff_in_vlan_test: wrong sub_opt  %d ",sub_opt);
      return -1;
    break;
  }
      
  if(sub_opt == 0 || sub_opt == 1)
    rtux_feature_ctrl(0 /*mr*/, 
                      0 /*mac_ptp*/, 
                      0/*mac_ll*/, 
                      1/*mac_single*/, 
                      0/*mac_range*/, 
                      0/*mac_br*/,
                      0/*drop when full_match full*/);
  else if(sub_opt == 2 || sub_opt == 3)
    rtux_feature_ctrl(0 /*mr*/, 
                      0 /*mac_ptp*/, 
                      0/*mac_ll*/, 
                      0/*mac_single*/, 
                      1/*mac_range*/, 
                      0/*mac_br*/,
                      0/*drop when full_match full*/);
  else if(sub_opt == 4)                     
    rtux_feature_ctrl(0 /*mr*/, 
                      0 /*mac_ptp*/, 
                      0/*mac_ll*/, 
                      0/*mac_single*/, 
                      0/*mac_range*/, 
                      1/*mac_br*/,
                      0/*drop when full_match full*/);

  
  return 0;
}

/**
 * opt=7
 */
int config_hp_test(int sub_opt, int port_num)
{
  int i;
  int pvid = 1;
  tru_disable();
  // for snake test
  
  vlan_entry_vd( 0,          //vid, 
                 0x3,  //port_mask, 
                 0,      //fid, 
                 0,            //prio,
                 0,            //has_prio,
                 0,            //prio_override, 
                 0             //drop
                );    

  vlan_entry_vd( 1,          //vid, 
                 0xF,  //port_mask, 
                 1,      //fid, 
                 0,            //prio,
                 0,            //has_prio,
                 0,            //prio_override, 
                 0             //drop
                );      
  rtux_add_ff_mac_single(0/*ID*/, 1/*valid*/, mac_single_spirent_A/*MAC*/);
  rtux_add_ff_mac_single(1/*ID*/, 1/*valid*/, mac_single_spirent_B/*MAC*/);  

  ep_set_vlan(0 /*port*/, 0/*access port*/, 1 /*fix_prio*/, 7 /*prio_val*/, 1 /*pvid*/);
  ep_set_vlan(1 /*port*/, 0/*access port*/, 1 /*fix_prio*/, 7 /*prio_val*/, 1 /*pvid*/);  
//   ep_set_vlan(0 /*port*/, 0/*access port*/, 1 /*fix_prio*/, 0 /*prio_val*/, 1 /*pvid*/);
//   ep_set_vlan(1 /*port*/, 0/*access port*/, 1 /*fix_prio*/, 0 /*prio_val*/, 1 /*pvid*/);

  ep_set_vlan(2 /*port*/, 0/*access port*/, 0 /*fix_prio*/, 0 /*prio_val*/, 1 /*pvid*/);
  ep_set_vlan(3 /*port*/, 0/*access port*/, 0 /*fix_prio*/, 0 /*prio_val*/, 1 /*pvid*/);  
//   ep_set_vlan(2 /*port*/, 0/*access port*/, 1 /*fix_prio*/, 7 /*prio_val*/, 1 /*pvid*/);
//   ep_set_vlan(3 /*port*/, 0/*access port*/, 1 /*fix_prio*/, 7 /*prio_val*/, 1 /*pvid*/);

  ep_vcr1_wr( 0 /*port*/, 1/*is_vlan*/, 0 /*address*/, 0xFFFF /*data */ ); 
  ep_vcr1_wr( 1 /*port*/, 1/*is_vlan*/, 0 /*address*/, 0xFFFF /*data */ );      
  ep_vcr1_wr( 2 /*port*/, 1/*is_vlan*/, 0 /*address*/, 0xFFFF /*data */ ); 
  ep_vcr1_wr( 3 /*port*/, 1/*is_vlan*/, 0 /*address*/, 0xFFFF /*data */ ); 
  
  rtux_feature_ctrl(0 /*mr*/, 
                    0 /*mac_ptp*/, 
                    0/*mac_ll*/, 
                    1/*mac_single*/, 
                    0/*mac_range*/, 
                    0/*mac_br*/,
                    0/*drop when full_match full*/);  
  
   rtux_set_hp_prio_mask(1<<0); // 7 prio_val
//   rtux_set_hp_prio_mask(1<<7); // 0 prio_val
  
  if(sub_opt == 1)
    tatsu_drop_nonHP_enable();

  
  return 0;
}


int config_startup(int opt, int sub_opt, int port_num)
{
 
   TRACE(TRACE_INFO,"config_startup: opt = %ds sub_opt = %d ",opt, sub_opt);
  switch(opt)
  {
    case 1:
      config_default(sub_opt,port_num);
      break;
    case 2:
      config_tru_demo_test(sub_opt,port_num);
      break;
    case 3:
      config_snake_standard_traffic_test(sub_opt,port_num);
      break;
    case 4:
      config_snake_ff_traffic_test(sub_opt,port_num);
      break;
    case 5:
      config_VLAN_test(sub_opt, port_num);
      break;
    case 6:
      config_snake_ff_hacked_traffic_test(sub_opt,port_num);
      break;
    case 7:
      config_hp_test(sub_opt,port_num);
      break;
    //////////////////////////////////////////////////////////////////
    case 0:
    default:
      config_info();
      exit(1);
      break;    
  }
  
}
