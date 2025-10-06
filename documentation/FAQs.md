# ❓ Frequently Asked Questions (FAQs)

## What should I do if I stop receiving the daily emails?

If no `.csv` files are being received, restart the **main module**:

- **Quart installation:** switch off the large metal control box (the main module).  
- **Antisa installation:** switch off the single module provided for that site.  

In both cases, toggle the **I1 switch** (bottom-right in *“Connections Diagram.pdf”*) **off for 5 seconds**, then turn it back on.  
The system will reboot and resume normal operation.  

*If you have a computer with the Arduino IDE installed (see documentation for setup), you can connect via USB to monitor sensor updates every 5 seconds.*

---

## What if a sensor shows abnormal or negative readings, but emails are still being received?

If the data email arrives but one sensor is returning impossible values (e.g., negative EC or unstable pH readings), you only need to restart the affected **pool module**:

- Identify which pool is sending the incorrect values.  
- On that pool’s box, toggle the **I1 switch** (bottom-right) off for 5 seconds, then turn it back on.  

The module will reset and resume normal readings.  
No calibration is needed after this restart.

---

## How can I calibrate the sensors?

Calibration procedures for DO, EC, and pH sensors are explained in:  
📄 `/documentation/calibrating_sensors.pdf`

---

## How often should sensors be calibrated?

Calibration frequency depends on the sensor type and environmental conditions.  
Refer to the manufacturer and project guidelines available in:  
📁 `/sensors_and_controllers/`
