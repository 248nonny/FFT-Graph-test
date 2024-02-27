pactl load-module module-null-sink channels=3 channel_map=left,center,right rate=44100 sink_name=wavetrace sink_properties="device.description='WaveTrace'"
pactl load-module module-loopback source=alsa_input.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.2.analog-mono sink=wavetrace channel_map=left
pactl load-module module-loopback source=alsa_input.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.3.analog-mono sink=wavetrace channel_map=center
pactl load-module module-loopback source=alsa_input.usb-C-Media_Electronics_Inc._USB_PnP_Sound_Device-00.analog-mono sink=wavetrace channel_map=right
