# Parametreler ve telemetri

Başlangıç noktası: `sim/SIM_satellite/RUN_nominal/input.py`.
Python girdileri derleme gerektirmez; `models/` altındaki C++ değişikliği gerektirir.
Tüm değerler eğitim için seçilmiş varsayımlardır, bir şirketin uydu verileri değildir.

| Nesne / alan | Varsayılan | Açıklama |
|---|---:|---|
| dyn.satellite.mass | 12 kg | Sürükleme ve ışınım ivmeleri için kütle |
| altitude0 / eccentricity | 400 km / 0 | Perigee yüksekliği / eliptiklik |
| inclination_deg / raan_deg | 51.6° / 25° | Eğim ve düğüm yönü |
| arg_perigee_deg / true_anomaly_deg | 0° / 0° | Perigee yönü ve başlangıç konumu |
| drag_area / srp_area | 0.06 / 0.12 m² | Sabit etkin kuvvet alanları |
| cd / cr | 2.2 / 1.3 | Sürükleme/ışınım katsayıları |
| density_scale | 1 | Nominal yoğunluk duyarlılık çarpanı |
| dyn.spacecraft.eps.capacity_wh | 80 Wh | Kullanılabilir enerji kapasitesi |
| initial_soc | 0.65 | Başlangıç doluluğu |
| panel_area | 0.12 m² | Etkin fotovoltaik alan |
| panel_efficiency | 0.28 | Sabit PV verimi |
| incidence_cosine | 0.7 | Verilen geliş açısı kosinüsü; ADCS çıktısı değil |
| regulation_efficiency | 0.9 | Panelden bus'a verim |
| charge_efficiency / discharge_efficiency | 0.95 / 0.95 | Batarya dönüşüm verimleri |
| max_charge_w / max_discharge_w | 30 / 40 W | Güç sınırları |
| base_load_w | 8 W | Platformun sürekli güç talebi |
| cutoff_soc / recovery_soc | 0.20 / 0.35 | Histerezis eşikleri |
| dyn.spacecraft.payload.active_w / standby_w | 28 / 2 W | Modların güç talebi |
| first_on_s / cycle_s / on_duration_s | 600 / 1800 / 900 s | Komut zaman planı |

`RUN_low_power`: aynı fizik modeli; başlangıç SOC=0.22, panel alanı=0.025 m².
Bu bir operasyon/parametre senaryosudur; henüz hata enjeksiyon altyapısı yoktur.

## İzlenecek alanlar

- Yörünge: `pos`, `vel`, `altitude`, `illumination`, kuvvet büyüklükleri.
- EPS: `soc`, `energy_wh`, `solar_w`, `requested_w`, `served_w`, `unserved_w`.
- Batarya/güç dengesi: `charge_w`, `discharge_w`, `curtailed_w`, `loss_w`.
- Doğrulama: `balance_error_wh`, `bus_residual_w`.
- Faydalı yük: `commanded`, `mode` (0 kapalı, 1 bekleme, 2 aktif), `consumed_w`, `active_seconds`.
- Koordinasyon: `low_power`, `power_limited`.

Tam alan listesi `record.py` ve C++ başlıklarındadır. CSV sütunları isimden okunur;
sütun sırasına bağımlı dış uygulama yazılmamalıdır.
