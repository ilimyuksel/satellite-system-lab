# Üç düğümlü termal model — M2

Gövde, batarya ve faydalı yük ayrı sıcaklık durumlarıdır. Parametreler sentetiktir;
gerçek uydu termal tasarımının veya testle kalibre edilmiş bir modelin yerine geçmez.

## Fizik ve bağlantılar

Her düğüm için `m cp dT/dt = gelen ısı - çıkan ısı` uygulanır.
Gövde–batarya ve gövde–yük iletimi `G (T_gövde - T_düğüm)` olur.
Gövdenin net ışınımı `epsilon sigma A (T_gövde^4 - T_uzay^4)` olarak çıkarılır.
Güneş girdisi `1361 (AU/d)^2 A_güneş alpha görünürlük`, Dünya IR girdisi
`earth_ir_w_m2 × earth_view_factor × radiating_area × emissivity` olur.

- Gerçekte beslenen platform taban yükü ve batarya dönüştürücü kayıpları gövdeyi ısıtır.
- Batarya I²R kaybı ve gerçekte beslenen ısıtıcı bataryayı ısıtır.
- Faydalı yük tüketiminin `heat_fraction` oranı yük düğümünü ısıtır.
- Batarya sıcaklığı termostat talebini belirler. EPS önceliği platform → ısıtıcı → faydalı yüktür.
- Isıtıcı talebi güç sağlandığı anlamına gelmez; kısmi veya sıfır besleme mümkündür.

Termal durumlar 1 Hz RK4 ile ilerletilir. İletim ve ışınım ara aşamalarda yeniden
hesaplanır; çevre ve elektrik kaynakları bir sonraki güncellemeye kadar tutulur.
Başlangıç güncellemesinde zaman ilerlemez. Isı dengesi kalıntısı, depolanan ısı
değişimi ile aynı RK4 aşamalarında integre edilen net sınır girdisinin farkıdır.

## Varsayılan parametreler

`RUN_nominal/input.py` içindeki termal ayarlar değiştirilir. Dizi sırası gövde, batarya, yüktür.

| Alan | Varsayılan | Birim |
|---|---|---|
| `mass` | 8, 2, 2 | kg |
| `cp` | 900, 1000, 800 | J/(kg K) |
| `initial_k` | 293.15, 293.15, 293.15 | K |
| `conductance_battery` | 0.6 | W/K |
| `conductance_payload` | 0.8 | W/K |
| `solar_area` | 0.06 | m², PV dışı etkin izdüşüm |
| `absorptivity` | 0.6 | — |
| `radiating_area` | 0.25 | m² |
| `emissivity` | 0.75 | — |
| `earth_ir_w_m2` | 237 | W/m² |
| `earth_view_factor` | 0.3 | — |
| `space_k` | 3 | K |
| `heater_enabled` | true | — |
| `heater_on_k` / `heater_off_k` | 278.15 / 283.15 | K (5 / 10 °C) |
| `heater_rating_w` | 8 | W |

Kütlelerin toplamı varsayılanda 12 kg’dır. Yörünge kütlesiyle bu değerler otomatik
eşitlenmez; senaryo değiştirirken birlikte tutarlı seçilmelidir.

## Deney ve doğrulama

`cold` başlangıç sıcaklıkları 275.15 / 268.15 / 280.15 K'dir.
`cold_no_heater` aynı koşulu ısıtıcı kapalı çalıştırır. Her koşu 12000 saniyedir.
Testler analitik iletim çözümü, sabit güçle sıcaklık artışı, ışınım dengesi,
termostat histerezisi, boş bataryada ısı üretilmemesi ve ısı dengesini kapsar.
Soğuk koşulda 600. saniyede ısıtıcılı bataryanın daha sıcak ve SOC’sinin daha düşük
olduğu da kontrol edilir. Sonuçlar [doğrulama kaydında](VALIDATION.md).

## Sınırlar

Alanlar ve Dünya görüş faktörü sabittir; yönelimden türetilmez. Albedo, panel termal
düğümü, temas geometrisi, çok katmanlı yalıtım ve sıcaklığa bağlı malzeme özellikleri yoktur.
Batarya sıcaklığı henüz direnç veya kapasiteyi değiştirmez. PV/regülatörün panel
üzerindeki kayıpları bu termal ağa dahil değildir; yükün ısıya ayrılmayan enerji
payı modellenen düğümlerin dışına çıkar. Bu nedenle doğrulanan denge tanımlanan
üç düğümün sınırına aittir, tüm uydunun ayrıntılı enerji bütçesi değildir.

Kavramsal kaynak: [NASA Small Spacecraft Technology — Thermal Control](https://www.nasa.gov/smallsat-institute/sst-soa/thermal-control/).
