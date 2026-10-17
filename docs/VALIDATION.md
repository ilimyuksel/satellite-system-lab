# Doğrulama kaydı — 17 Ekim 2026

Ortam: macOS Apple Silicon, NASA Trick 25.1.2-beta. Aşağıdaki değerler bu proje
klasöründe yürütülen koşulardan alınmıştır; gerçek uydu gözlemiyle doğrulama değildir.

## C++ testleri

- Kuvvet yönü, kütle/alan ölçeklemesi, dönen atmosfer, gölge ve efemeris kapsamı: PASS.
- EPS enerji dengesi, dolu/boş batarya, geçersiz parametre ve histerezis: PASS.

## Bütünleşik koşular

- **nominal**: başlangıç SOC %65.00, son SOC %33.35; aktif yük 6000 s; en büyük enerji kalıntısı 4.661e-12 Wh.
- **low_power**: başlangıç SOC %22.00, son SOC %0.00; aktif yük 0 s; en büyük enerji kalıntısı 4.359e-12 Wh.

Her iki koşuda 12000 saniye / 2401 kayıt; SOC sınırları, gölgede sıfır panel
gücü, bus dengesi, komut/gerçek mod ve düşük SOC kısıtlaması kontrol edildi.

## Yörünge regresyonu

- Analitik iki-cisim maksimum konum farkı (m): 4.85471e-06
- J2 bağıl enerji kayması: 1.36701e-14
- 1 s / 0.5 s maksimum konum farkı (m): 4.34806e-06
- Bağımsız DOP853 maksimum konum farkı (m): 0.000489354

Referans çözüm aynı çevre varsayımlarını ve efemerisi kullanır; sayısal uygulamayı
sınar, atmosfer varsayımının gerçek doğruluğunu kanıtlamaz. EPS doğrulaması,
örneklenmiş ve sabit verimli enerji deposu denklemlerinin tutarlılığını sınar;
gerçek batarya karakteristiğini doğrulamaz.

## Web durumu

Gömülü verinin boyutu/sütunları, JavaScript sözdizimi ve kullanılan element
kimlikleri kontrol edildi. Tarayıcıda görsel doğrulama henüz tamamlanmadı.

Tekrar üret: `./tools/check.sh`. Yeni raporlar `artifacts/` altında oluşur.
Bu dokümandaki sayılar bir sürüm anlık görüntüsüdür; parametre değişikliğinde yenilenmelidir.
