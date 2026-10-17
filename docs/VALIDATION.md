# Doğrulama kaydı — 17 Ekim 2026

Ortam: macOS Apple Silicon, NASA Trick 25.1.2-beta. Aşağıdaki değerler bu proje
klasöründe yürütülen koşulardan alınmıştır; gerçek uydu gözlemiyle doğrulama değildir.

## C++ testleri

- Kuvvet yönü, kütle/alan ölçeklemesi, dönen atmosfer, gölge ve efemeris kapsamı: PASS.
- EPS enerji dengesi, dolu/boş batarya, geçersiz parametre ve histerezis: PASS.

## Bütünleşik koşular

- **nominal**: başlangıç SOC %65.00, son SOC %32.57; aktif yük 6000 s; en büyük enerji kalıntısı 9.457e-12 Wh.
- **low_power**: başlangıç SOC %22.00, son SOC %0.00; aktif yük 0 s; en büyük enerji kalıntısı 3.936e-12 Wh.

Her iki koşuda 12000 saniye / 2401 kayıt; SOC sınırları, gölgede sıfır panel
gücü, bus dengesi, komut/gerçek mod ve düşük SOC kısıtlaması kontrol edildi.

## Yörünge regresyonu

- Analitik iki-cisim maksimum konum farkı (m): 4.85471e-06
- J2 bağıl enerji kayması: 1.36701e-14
- 1 s / 0.5 s maksimum konum farkı (m): 4.34806e-06
- Bağımsız DOP853 maksimum konum farkı (m): 0.000489354

Referans çözüm aynı çevre varsayımlarını ve efemerisi kullanır; sayısal uygulamayı
sınar, atmosfer varsayımının gerçek doğruluğunu kanıtlamaz. EPS doğrulaması,
örneklenmiş enerji deposu, sabit verimler ve Rint devre denklemlerinin tutarlılığını sınar;
gerçek batarya karakteristiğini doğrulamaz.

## Web durumu

Gömülü verinin boyutu/sütunları, JavaScript sözdizimi ve kullanılan element
kimlikleri kontrol edildi. Tarayıcıda görsel doğrulama henüz tamamlanmadı.

Tekrar üret: `./tools/check.sh`. Yeni raporlar `artifacts/` altında oluşur.
Bu dokümandaki sayılar bir sürüm anlık görüntüsüdür; parametre değişikliğinde yenilenmelidir.


M1.1 ek kontrolleri: akım sınırlaması, alt gerilim kesmesi, şarj üst gerilim sınırı,
kapasite sağlık oranı, pozitif I²R kaybı, direnç sıfırken ideal modele dönüş ve aktif
süreye bağlı veri miktarı doğrulandı. Başlık değişiminden sonra eski Trick IO
alan konumlarının kullanılmasını önlemek için derleme hazırlık adımı eklendi.

## M2 — Termal doğrulama

`tools/check.sh` tamamlandı: üç C++ test programı ve sekiz Trick koşusu
(nominal, low_power, cold, cold_no_heater, validation, j2, drag, halfstep) geçti.
Analitik iletim, sabit ısı girdisi, ışınım dengesi, termostat histerezisi ve
enerjisiz bataryada ısıtıcıya güç verilmemesi kontrol edildi.

| Senaryo | Son gövde / batarya / yük (°C) | Isıtıcı enerjisi (Wh) | En büyük ısı kalıntısı (J) |
|---|---|---:|---:|
| nominal | 7.010 / 12.393 / 29.439 | 0 | 3.80e-8 |
| low_power | -2.559 / 5.725 / 4.749 | 0 | 1.37e-8 |
| cold | 1.667 / 5.630 / 22.909 | 9.8533 | 7.75e-9 |
| cold_no_heater | 0.531 / 3.520 / 21.763 | 0 | 1.96e-8 |

Dört sistem koşusunda 2401 kayıt kontrol edildi. 600. saniyede soğuk başlangıçta
ısıtıcılı batarya, ısıtıcısız koşula göre daha sıcak ve daha düşük SOC değerindedir.
Elektrik dağıtımı ile düğümlere aktarılan ısının tutarlılığı sınandı.
Bu sonuçlar denklemlerin sayısal tutarlılığını gösterir; uçuş verisiyle doğrulama değildir.
