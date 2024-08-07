/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

//sssD

.section .data
        
  .global saveicon
  .global load_symbol
  .global player_tim
  .global player_tunic_green_tim 
  .global player_agm
  .global player_anm
  .global sword_sg2
  .global shield_sgm2
  //.global scene_tim
  //.global iwakabe_tim
  //.global iwakabe_far_tim
  //.global iwayuka_tim
  .global deku_shrine_block000a
  .global deku_shrine_block000b
  .global deku_shrine_block000b_000
  .global deku_shrine_block000b_001
  .global deku_shrine_block000b_002
  .global deku_shrine_block000b_003
  .global scene_col
  .global scene_room00_sg2
  //.global scene_room01_sg2
  //.global scene_room02_sg2
  //.global scene_room03_sg2
  //.global scene_room04_sg2
  //.global scene_room05_sg2
  //.global scene_room05_sg2_ext
  .global obj_syokudai_tim
  .global obj_syokudai_sgm2
  .global obj_flame_tim
  .global obj_flame_pal_tim
  .global obj_flame_sgm2
  .global obj_flame_high_sgm2
  .global obj_tsubo_tim
  .global obj_tsubo_sgm2
  .global obj_tsubo_low_sgm2
  .global obj_grass_tim

  .global obj_grass_sgm2
  .global obj_grass_low_sgm2
  .global obj_grass_low2_sgm2
  .global obj_grass_cut_tim
  .global obj_grass_cut_full_sgm2
  .global obj_grass_cut_half_sgm2
  .global obj_dangeon_door_tim
  .global screen_lifebar_tim
  .global main_button_bg
  .global main_button_labels
  .global button_action_labels
  .global action_icon_bomb
  .global action_icon_hookshot
  .global icon_256_a_16_tim
  .global icon_256_b_16_tim
  .global obj_dekunuts_plant_tim
  .global obj_dekunuts_body_tim
  .global obj_dekunuts_plant_sgm2
  .global dekunuts_body_static_sgm2
  .global dekunuts_agm
  .global dekunuts_anm
  .global obj_item_sphere_sgm2
  .global obj_item_sphere_low_sgm2
  .global obj_item_sphere_tim
  .global texture_data_start
  .global texture_data_end

player_agm:
  //.incbin "../AGAME_PCDATA/player_new.agm"
  .incbin "../zelda_assets/models/link_reference_new_clean.agm" #_global_normal
  .balign 16

player_anm: 
  .incbin "../zelda_assets/models/link_reference_new_clean.agm.anm" #_global_normal
  .balign 16

sword_sg2:
  .incbin "../AGAME_PCDATA/prop_swerd.sgm2"
  .balign 16

shield_sgm2:
  .incbin "../zelda_assets/models/hyral_shield.sgm2"
  .balign 16

scene_room00_sg2:
  .incbin "../zelda_assets/scenes/test_dampei/room_000.sgm2"
  .balign 16
/*
scene_room01_sg2:
  .incbin "../AGAME_PCDATA/Map/Test/room_001.sgm2"
  .balign 16

scene_room02_sg2:
  .incbin "../AGAME_PCDATA/Map/Test/room_002.sgm2"
  .balign 16

scene_room03_sg2:
  .incbin "../AGAME_PCDATA/Map/Test/room_003.sgm2"
  .balign 16

scene_room04_sg2:
  .incbin "../AGAME_PCDATA/Map/Test/room_004.sgm2"
  .balign 16

scene_room05_sg2:
  .incbin "../AGAME_PCDATA/Map/Test/room_005.sgm2"
  .balign 16

scene_room05_sg2_ext:
  .incbin "../AGAME_PCDATA/Map/Test/room_005-external.sgm2"
  .balign 16
*/
scene_col:
  //.incbin "../AGAME_PCDATA/Map/Test/Collision/map_collision.col2" 
  .incbin "../zelda_assets/scenes/test_dampei/collision/complete_test_room_deku_shrine.col2    "
  .balign 16

obj_flame_sgm2:
  .incbin "../AGAME_PCDATA/obj_flame.sgm2"
  .balign 16

obj_flame_high_sgm2:
  .incbin "../AGAME_PCDATA/obj_flame_high.sgm2"
  .balign 16

obj_tsubo_sgm2:
  .incbin "../AGAME_PCDATA/obj_tsubo.sgm2"
  .balign 16

obj_tsubo_low_sgm2:
  .incbin "../AGAME_PCDATA/obj_tsubo_low.sgm2"
  .balign 16

obj_grass_sgm2:
  .incbin "../AGAME_PCDATA/obj_grass.sgm2"
  .balign 16

obj_grass_low_sgm2:
  .incbin "../AGAME_PCDATA/obj_grass_low.sgm2"
  .balign 16

obj_grass_low2_sgm2:
  .incbin "../AGAME_PCDATA/obj_grass_low2.sgm2"
  .balign 16

obj_grass_cut_full_sgm2:
  .incbin "../AGAME_PCDATA/obj_grass_cut_full.sgm2"
  .balign 16

obj_grass_cut_half_sgm2:
  .incbin "../AGAME_PCDATA/obj_grass_cut_half.sgm2"
  .balign 16

texture_data_start:
saveicon:
  .incbin "../AGAME_PCDATA/saveicon.tim"
  .balign 16

load_symbol:
  .incbin "../AGAME_PCDATA/graphics/system/LOAD_SYMBOL.TIM"
  .balign 16

player_tim:
  .incbin "../AGAME_PCDATA_OLD/4bit-test/link/parts/link_area_composite.TIM"
  .balign 16

player_tunic_green_tim:
  .incbin "../AGAME_PCDATA_OLD/4bit-test/link/parts/green_tunic_pal.TIM"
  .balign 16
/*
scene_tim:
  .incbin "../AGAME_PCDATA/graphics/replace_block-psnew2225.tim"
  .balign 16*/
/*
iwakabe_tim:
  .incbin "../AGAME_PCDATA/iwakabeyuka_composite_v2.tim"
  .balign 16

iwakabe_far_tim:
  .incbin "../AGAME_PCDATA/iwakabe_4bit-far.tim"
  .balign 16

iwayuka_tim:
  .incbin "../AGAME_PCDATA/iwayuka_4bit.tim"
  .balign 16
*/
deku_shrine_block000a:   
  .incbin "../zelda_assets/textures_png/scenes/deku_shrine_block000a.tim"
  .balign 16

deku_shrine_block000b:
  .incbin "../zelda_assets/textures_png/scenes/deku_shrine_block000b.tim"
  .balign 16

deku_shrine_block000b_000:
  .incbin "../zelda_assets/textures_png/scenes/deku_shrine_block000b_000.tim"
  .balign 16

deku_shrine_block000b_001:
  .incbin "../zelda_assets/textures_png/scenes/deku_shrine_block000b_001.tim"
  .balign 16

deku_shrine_block000b_002:
  .incbin "../zelda_assets/textures_png/scenes/deku_shrine_block000b_002.tim"
  .balign 16

deku_shrine_block000b_003:
  .incbin "../zelda_assets/textures_png/scenes/deku_shrine_block000b_003.tim"
  .balign 16

obj_syokudai_tim:
  .incbin "../AGAME_PCDATA/obj_syokudai.tim"
  .balign 16

obj_syokudai_sgm2:
  .incbin "../AGAME_PCDATA/obj_syokudai.sgm2"
  .balign 16

obj_flame_tim:
  .incbin "../AGAME_PCDATA/obj_flame.tim"
  .balign 16

obj_flame_pal_tim:
  .incbin "../AGAME_PCDATA/obj_flame_pa-24.tim"
  .balign 16

obj_tsubo_tim:
  .incbin "../AGAME_PCDATA/obj_tsubo.tim"
  .balign 16

obj_grass_tim:
  .incbin "../AGAME_PCDATA/obj_grass_multiclut.tim"
  .balign 16

obj_grass_cut_tim:
  .incbin "../AGAME_PCDATA/obj_grass_cut.tim"
  .balign 16

obj_dangeon_door_tim:
  .incbin "../AGAME_PCDATA/obj_dangeon_door.tim"
  .balign 16

screen_lifebar_tim:
  .incbin "../AGAME_PCDATA/screen_lifebar2.tim"
  .balign 16

main_button_bg:
  .incbin "../AGAME_PCDATA/ui/modern/button_back_new-or8.tim"
  .balign 16

main_button_labels:
  .incbin "../AGAME_PCDATA/ui/modern/button_labels-or8.tim"
  .balign 16
  
button_action_labels:
  .incbin "../AGAME_PCDATA/ui/modern/button_action_labels_t-or8.tim"
  .balign 16

action_icon_bomb:
  .incbin "../AGAME_PCDATA/ui/modern/action_icon_bomb-or8.tim"
  .balign 16
  
action_icon_hookshot:
  .incbin "../AGAME_PCDATA/ui/modern/action_icon_hookshot-or8.tim"
  .balign 16

icon_256_a_16_tim:
  .incbin "../AGAME_PCDATA/icon_256_a-16.tim"
  .balign 16

icon_256_b_16_tim:
  .incbin "../AGAME_PCDATA/icon_256_b-16.tim"
  .balign 16
/*
 * Objects - Enemies
 */
obj_dekunuts_plant_tim:
  .incbin "../AGAME_PCDATA/objects/enemy/dekunuts/obj_dekunuts_plant.tim"
  .balign 16
obj_dekunuts_body_tim:
  .incbin "../AGAME_PCDATA/objects/enemy/dekunuts/obj_dekunuts_body.tim"
  .balign 16
obj_dekunuts_plant_sgm2:
  .incbin "../AGAME_PCDATA/objects/enemy/dekunuts/dekunuts_plant.sgm2"
  .balign 16
dekunuts_body_static_sgm2:
  .incbin "../AGAME_PCDATA/objects/enemy/dekunuts/dekunuts_body_static.sgm2"
  .balign 16
dekunuts_agm:
  .incbin "../zelda_assets/models/enemy/dekunuts/dekunuts_new.agm"
  .balign 16
dekunuts_anm:
  .incbin "../zelda_assets/models/enemy/dekunuts/dekunuts_new.agm.anm"
  .balign 16
obj_item_sphere_sgm2:
  .incbin "../zelda_assets/models/item_sphere.sgm2"
  .balign 16
obj_item_sphere_low_sgm2:
  .incbin "../zelda_assets/models/item_sphere_low.sgm2"
  .balign 16
obj_item_sphere_tim:
  .incbin "../zelda_assets/textures_png/reflection_sphere.tim"
  .balign 16
texture_data_end: 