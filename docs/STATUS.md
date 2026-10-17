# Kapsam ve mevcut durum

Bu tablo uygulanan davranışı gösterir; gelecekteki hedefleri tamamlanmış saymaz.

| Kimlik | Özellik | Durum | Kanıt |
|---|---|---|---|
| ORB-01 | 3B konum/hız, Trick RK4 | Çalışıyor | Analitik ve DOP853 karşılaştırması |
| ENV-01 | J2, sürükleme, SRP, Güneş/Ay | Çalışıyor, sadeleştirilmiş | Kuvvet yönü/ölçekleme ve yörünge testleri |
| ENV-02 | Tam ve yarı gölge | Çalışıyor | Gölge geometri testleri |
| EPS-01 | Aydınlanmaya bağlı panel gücü | Çalışıyor; açı katsayısı sabit | Gölgedeki üretimin sıfır olması |
| EPS-02 | Batarya enerji ve SOC hesabı | Çalışıyor; enerji deposu modeli | Enerji dengesi, dolu/boş sınır testleri |
| EPS-03 | Güç sınırı, düşük SOC, yük kısıtlama | Çalışıyor | Birim testleri ve low_power koşusu |
| PAY-01 | Planlı çalışma/bekleme/kapalı modu | Çalışıyor | Komut ile gerçek tüketim ayrı kaydediliyor |
| INT-01 | Çevre → EPS → faydalı yük | Çalışıyor | İki senaryonun bütünleşik testleri |
| WEB-01 | 3B kayıt oynatımı ve güç/SOC grafikleri | Hazır; tarayıcıda görsel QA bekliyor | Kaynak ve gömülü veri kontrolleri |
| ADCS-01 | Quaternion, atalet, kontrol torku | Henüz yok | — |
| THM-01 | Termal düğümler ve ısı dengesi | Henüz yok | — |
| PAY-02 | Ölçüm/görüntü/veri üretimi | Henüz yok | — |
| FAULT-01 | Parametrik hata enjeksiyonu | Henüz yok | Düşük enerji koşusu hata enjeksiyonu değildir |
| AI-01 | Anomali tespiti ve veri seti ayrımları | Henüz yok | — |

## Sonraki geliştirme sırası

1. Mentörle ilk kapsamı değerlendirme; sentetik parametreleri ve kabul ölçütlerini netleştirme.
2. Batarya/gövde/faydalı yük için küçük termal ağ ve enerji alışverişi.
3. Yönelim dinamiği ve basit ADCS; verilen panel açısının yerine hesaplanan açı.
4. Doğrulanmış alt sistemler üzerinde hata enjeksiyonu ve telemetri deneyleri.

Bunlar planlanan işlerdir. Bu sürümün amacı ilk uçtan uca fiziksel bağlantıyı göstermek.
