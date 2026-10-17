# Mentöre 3–5 dakikalık gösterim

## Söylenebilecek somut özet

> NASA Trick üzerinde çalışan bir C++ yörünge/çevre modelini basit elektrik güç
> sistemi ve faydalı yükle birleştirdik. Gölgeye giriş panel üretimini, üretim ve
> yükler batarya enerjisini, akım/gerilim sınırları ve enerji durumu da faydalı yükün çalışmasını etkiliyor.
> Dört sistem senaryosunu çalıştırdık ve enerji dengesi ile sınır davranışlarını test ettik.

## Gösterim sırası

1. README kapsam tablosunu göster; ADCS’nin henüz olmadığını belirt.
2. `./tools/run.sh nominal` çalıştır. Çıktıdaki `web/index.html` dosyasını aç.
3. Oynat'a bas. Küreyi döndür; bunun kaydedilmiş Trick sonuçları olduğunu söyle.
4. Güneş simgesini ve Dünya’nın aydınlık/karanlık tarafını göster. Uydu gölgeye girerken panel üretimini ve şarj/deşarj göstergesini izle. Güneş uzaklığı görselde ölçekli değildir. Aydınlıkta da tüketim üretimi aşarsa batarya boşalabilir.
5. Faydalı yükün zaman planıyla beklemeden çalışmaya geçmesini göster.
6. `./tools/run.sh low_power` çalıştırıp sekmeyi yenile. Düşük enerji/yük kısıtlamasını göster.
7. `docs/VALIDATION.md` ve `artifacts/system-validation.json` sonuçlarını göster.
8. `./tools/run.sh cold` ve `./tools/run.sh cold_no_heater` koşularını karşılaştır. Isıtıcının batarya sıcaklığını yükseltirken elektrik tükettiğini göster.
9. Sıcaklık eğrilerini ve `artifacts/thermal-validation.json` raporunu göster. Sonraki adım hesaplanan yönelim ve ADCS.

## Söylenmemesi gerekenler

- "Gerçek uydu telemetrisi kullandık." → Veriler sentetik.
- "Tüm uydu sistemlerini modelledik." → İlk alt sistem bağlantıları var.
- "3B gövdenin yönelimi ADCS sonucu." → Gövde simgesi temsili.
- "Batarya modeli hücre seviyesinde doğru." → Enerji deposu yaklaşımı.
- "Yapay zekâ anomalileri buluyor." → Henüz bu aşamada değiliz.
- "Oynat dediğimizde Trick canlı çalışıyor." → Tamamlanmış koşu oynatılıyor.

## Kodun nereden anlatılacağı

`S_define` → `models/system/src/Spacecraft.cpp` → `models/power/src/Power.cpp`
→ `RUN_nominal/input.py` → `record.py` sırası, fizik ve yazılım bağlantısını açıklar.
