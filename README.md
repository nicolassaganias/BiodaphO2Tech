# 🧪 BiodaphO₂ Technology

## Overview

**BiodaphO₂ Technology** is a modular environmental monitoring system designed for **wastewater treatment experiments** with *Daphnias* in open-air ponds.  
It continuously records physicochemical parameters and **sends a single `.csv` file every 24 hours** summarizing all daily data.

The system operates autonomously using an **Arduino Opta Wi-Fi PLC** with an **analog expansion module**, **industrial Atlas Scientific sensors**, **flow meters**, and a **turbidity sensor**, all connected through an **Outdoor 4G Router**.

---

## System Architecture

### **Main Installation (Quart, Spain)**

- 2 × **Pool Modules**  
  Each pool has:
  - 1 × pH Sensor (Atlas Scientific Industrial)  
  - 1 × Conductivity Sensor (Atlas Scientific Industrial)    
  - 1 × Dissolved Oxygen Sensor (Atlas Scientific Industrial)  

- 1 × **Main Control Box**
  - Arduino Opta Wi-Fi PLC  
  - Analog Extension Module  
  - 3 Flow Meters (general inlet + 2 pools)  
  - 1 Turbidity Sensor  
  - Outdoor 4G Router for data transmission  

### **Secondary Installation (Antisa, Greece)**
  - 1 × **Main Control Box**
  - Arduino Opta Wi-Fi PLC  
  - 1 × pH Sensor (Atlas Scientific Industrial)  
  - 1 × Conductivity Sensor (Atlas Scientific Industrial)   

All sensor data is collected by the main control unit, compiled into a `.csv` file, and sent via email every day.

---

Refer to /FAQs and /Documentation for further information and usage instructions.
