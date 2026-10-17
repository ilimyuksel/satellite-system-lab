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

## M1.1 — Bataryanın elektriksel davranışı

Parametre ve durum değişkeni aynı şey değildir. Örneğin iç direnç bir girdidir;
akım, uç gerilimi ve dirençte oluşan ısı bu girdilerden hesaplanan çıktılardır.
Senaryo girdileri `RUN_nominal/input.py` içinde açıkça atanır ve CSV'ye kaydedilir.

| EPS parametresi | Varsayılan | Fiziksel sonucu |
|---|---:|---|
| `capacity_soh` | 1.0 | Kullanılabilir kapasite = nominal Wh × sağlık oranı |
| `ocv_empty_v` | 12 V | Sıfır enerji oranında sentetik açık devre gerilimi |
| `ocv_full_v` | 16.8 V | Tam enerji oranında sentetik açık devre gerilimi |
| `internal_resistance` | 0.08 Ω | Akıma bağlı gerilim düşümü ve I²R kaybı |
| `min_terminal_v` | 11.2 V | Deşarj sırasında alt gerilim sınırı |
| `max_terminal_v` | 16.8 V | Şarj sırasında üst gerilim sınırı |
| `max_charge_a` | 2 A | Şarj akımı üst sınırı |
| `max_discharge_a` | 3 A | Deşarj akımı üst sınırı |
| `panel_health` | 1.0 | Panel çıkışına uygulanan sabit performans oranı |

Mevcut güç sınırları da geçerlidir. Akım, gerilim, dönüştürücü gücü ve kalan enerji
sınırlarından en kısıtlayıcı olanı uygulanır. `capacity_soh` ve `panel_health`
senaryo boyunca sabit katsayılardır; çevrim/yaşlanma fiziği veya hata enjeksiyonu değildir.

### Hesaplanan elektriksel çıktılar

- `effective_capacity_wh`: sağlık oranı uygulanmış kapasite.
- `ocv_v`: açık devre gerilimi; enerji doluluğunun doğrusal fonksiyonu.
- `terminal_v`: yük altındaki paket uç gerilimi.
- `battery_current_a`: pozitif deşarj, negatif şarj.
- `ohmic_heat_w`: iç dirençte oluşan ısı gücü.
- `converter_loss_w`: şarj/deşarj dönüştürücü kayıpları.
- `charge_limit_w`, `discharge_limit_w`: o andaki fiziksel sınırların izin verdiği bus güçleri.

**Doğruluk sınırı:** Bu, enerji deposuna bağlanmış durağan Rint devre yaklaşımıdır.
`soc` hâlâ enerji oranıdır (E/Emax), Coulomb sayımıyla bulunan yük SOC'si değildir.
Gerilim–enerji eğrisi sentetiktir; hücre veri sayfasından kalibre edilmemiştir.
RC kutuplanma dinamiği, hücre dengesizliği, sıcaklık bağımlılığı ve yaşlanma yoktur.

## M1.1 — Faydalı yük çıktıları

| Parametre | Varsayılan | Etki |
|---|---:|---|
| `data_rate_mbps` | 2 Mbit/s | Yalnızca gerçek aktif çalışma sırasında üretilen veri |
| `heat_fraction` | 0.9 | Gerçek elektrik tüketiminin yerel ısıya dönüşen oranı |

`generated_data_mbit` toplam üretilen veridir. Bellek doluluğu, downlink ve veri
kaybı modellenmez. `heat_w` bir termal kaynak çıktısıdır; sıcaklık değildir.
Batarya direnç ısısı batarya düğümüne, faydalı yük ısısı yük düğümüne bağlanmıştır.

## Duyarlılık deneyleri

- `internal_resistance`: 0.08 → 0.4 Ω; gerilim düşümü ve ısı kaybını karşılaştır.
- `max_discharge_a`: 3 → 0.5 A; güç yeterli görünse bile faydalı yükün neden reddedildiğine bak.
- `capacity_soh`: 1 → 0.7; aynı başlangıç enerji oranıyla daha az toplam enerji kaldığını incele.
- `panel_health`: 1 → 0.6; üretim ve enerji açığının değişimini izle.
- `data_rate_mbps`: 2 → 4; bu modelde enerji talebi sabit tutulur, yalnızca üretilen veri artar.
  Veri hızı–güç ilişkisi henüz cihaz modeliyle bağlanmamıştır.

Devre modelinin kavramsal referansı:
https://www.mathworks.com/help/simscape-battery/ref/batteryequivalentcircuit.html
Burada MATLAB/Simulink kullanılmaz; denklemler C++ ile uygulanır.

## M2 — Termal parametreler

Üç düğümün kütleleri, özgül ısıları, başlangıç sıcaklıkları, iletim katsayıları,
yüzey optik özellikleri ve termostat eşikleri [THERMAL.md](THERMAL.md) içinde listelenir.
