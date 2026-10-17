# Köken, kaynak ve atıflar

- Proje NASA'nın resmi ürünü veya NASA tarafından doğrulanmış bir uydu modeli değildir.
- NASA Trick bağımsız dış bağımlılıktır: https://github.com/nasa/trick . Trick kaynakları
  bu depo içine kopyalanmadı; kendi lisansı altında kullanılır.
- Yörünge altyapısı bu çalışma alanındaki önceki SIM_satellite_learning eğitim
  prototipinden taşındı. Trick integrasyon kullanımı yerel SIM_sat2d örneğinden öğrenildi.
- Güneş/Ay verileri: NASA/JPL Horizons, DE441. Ham yanıtlar ve sorgu kaydı
  `sim/SIM_satellite/environment/` altındadır.
- JPL API: https://ssd-api.jpl.nasa.gov/doc/horizons.html
- Kütleçekim sabitleri: https://ssd.jpl.nasa.gov/astro_par.html
- J2: https://ai-solutions.com/_freeflyeruniversityguide/j2_perturbation.htm
- Üstel atmosfer yaklaşımı: https://www.orekit.org/static/apidocs/org/orekit/models/earth/atmosphere/SimpleExponentialAtmosphere.html

Panel/batarya/faydalı yük parametreleri sentetik eğitim varsayımlarıdır.
Şirket kaynak kodu, özel parametreler veya operasyonel telemetri kullanılmaz.
Paylaşım lisansı henüz proje sahibi tarafından seçilmemiştir.
