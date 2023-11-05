#!/bin/bash
output_file="/home/lukas/drive_health_status.txt"
> "$output_file"  # This clears the content of the file without needing to touch it first.

drives=( /dev/sda /dev/sdb /dev/sdc /dev/sdd )
all_drives_ok=true

for drive in "${drives[@]}"; do
  result=$(smartctl -a "$drive" | grep "SMART overall-health self-assessment test result:")
  if ! echo "$result" | grep -q "PASSED"; then
    echo "Warning: $drive may be failing" >> "$output_file"
    all_drives_ok=false
  fi
done

if $all_drives_ok; then
    echo "All drives have a passing overall health."
else
    echo "Warning: One or more drives are failing. Check $output_file for more details."
fi

# Create a super user crontab entry like this for example, to run daily:
# 30 3 * * * /home/lukas/test_raid.sh

# Then add this to your ~/.bashrc, ~/.zshrc or otherwise and on login it
# inform you if any drives are failing:
# if [ -s /home/lukas/drive_health_status.txt ]; then
#  cat /home/lukas/drive_health_status.txt
#fi
