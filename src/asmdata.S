/*
 * A Game Engine
 *
 * (C) 2021 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

.section .data
        
  .global saveicon
  .global load_symbol
  .global player_tim
  .global player_agm
  .global player_anm
  .global sword_sg2
  .global scene_tim
  .global iwakabe_tim
  .global iwakabe_far_tim
  .global iwayuka_tim
  .global scene_col
  .global scene_room00_sg2
  .global scene_room01_sg2
  .global scene_room02_sg2
  .global scene_room03_sg2
  .global scene_room04_sg2
  .global scene_room05_sg2
  .global scene_room05_sg2_ext
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
  .global texture_data_start
  .global texture_data_end

player_agm:
  .incbin "../AGAME_PCDATA/player_new.agm"
  .balign 16

player_anm:
  .incbin "../AGAME_PCDATA/player_new.agm.anm"
  .balign 16

sword_sg2:
  .incbin "../AGAME_PCDATA/prop_swerd.sgm2"
  .balign 16

scene_room00_sg2:
  .incbin "../AGAME_PCDATA/Map/Test/room_000.sgm2"
  .balign 16

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

scene_col:
  .incbin "../AGAME_PCDATA/graphics/test_room_col_opt.col2"
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
  .incbin "../AGAME_PCDATA/graphics/player_area_composite.tim"
  .balign 16

scene_tim:
  .incbin "../AGAME_PCDATA/graphics/replace_block-psnew2225.tim"
  .balign 16

iwakabe_tim:
  .incbin "../AGAME_PCDATA/iwakabeyuka_composite_v2.tim"
  .balign 16

iwakabe_far_tim:
  .incbin "../AGAME_PCDATA/iwakabe_4bit-far.tim"
  .balign 16

iwayuka_tim:
  .incbin "../AGAME_PCDATA/iwayuka_4bit.tim"
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

texture_data_end: