# static-engine
Static-engine a.k.a SpearCutter

## 필요한 라이브러리 
1. [CppCore](https://github.com/profrog-jeon/cppcore): Unix & Window MultiPlatform Coding Lib  
유닉스에서 윈도우 API를 사용할 수 있게 하는 라이브러리로써 이를 통한 멀티플랫폼 개발이 가능하다.</br>
2. DocumentParser: MS-Office Document Parser   
Word, Excel, PowerPoint 문서들을 파싱할 수 있는 문서 파싱 라이브러리 이다.
Contact: profrog</br>

## Version
#### 1.0.0 : CVE-2022-30190(Follina)를 잡을 수 있도록 엔진 구성</br>
#### 1.0.1 : 파일이 제대로 다운받아지지 않는 문제 해결 및 예외처리 추가</br>
#### 1.1.0 : 문서내에 악성 매크로를 잡을 수 있도록 엔진 추가</br>
#### 1.2.0 : CVE-2017-11882(EQNEDT)를 잡을 수 있도록 엔진 추가</br>
#### 1.2.1 : ReMoteTemplateInjection을 잡을 수 있도록 엔진 수정</br>
#### 1.2.2 : ReMoteTemplateInjection을 잡지 못하던 버그 수정</br>
#### 1.2.3 : 파일이 제대로 다운로드 되지않는 문제에 대해 예외처리 추가</br>
#### 1.2.4 : [ZIPLib](https://github.com/kuba--/zip)에서 파일을 제대로 읽지 못하는 버그 수정 </br>
#### 1.2.5 : OLETOOLS를 추가하여 vba 매크로 탐지</br>
#### 1.3.0 : OLETOOLS를 추가하여 XLM 매크로 탐지</br>
#### 1.3.1 : 매크로 추출엔진에서 외부 링크가 나오는 버그 수정</br>
#### 1.3.2 : Base64 디코더 추가 </br>
#### 1.4.0 : Dynamic Data Exchang(DDE) 엔진 추가</br>
#### 1.4.1 : Dynamic Data Exchang(DDE) 엔진 버그 수정</br>
#### 1.4.2 : 악성 매크로 결과 문구 Malicious Macro -> Malicious Macro(VBA or XLM)으로 수정</br>
#### 1.5.0 : CVE-2017-0199 탐지포이늩 추가(word)</br>
#### 1.5.1 : CVE-2017-0199로 인해 URL을 정상적으로 뽑지 못하는 문제 수정</br>
#### 1.5.2 : OLETOOLS에 넘겨주는 파일인자에 대해 \이 빠져있는 문제 수정</br>
#### 1.5.3 : 잠겨있는 엑셀 파일에 대해서도 매크로 추출을 진행할 수 있도록 수정</br>
#### 1.5.4 : CVE-2017-11882를 제대로 잡지 못하는 문제 수정</br>
#### 1.5.5 : 전 엔진 리펙토링 및 성능개선 작업</br>
#### 2.0.0 : CPPCORE, DocumentParser를 사용하는 새로운 엔진으로 리펙토링 Nospear -> SpearCutter </br>
#### 2.0.1 : 엔진에서 서버로 보내는 정보가 누락되는 문제 해결</br>
#### 2.0.2 : 각종 버그사항 및 CheckInternalCVE엔진 탐지 및 </br>
