import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import WritePage from './Pages/WritePage/writePage';
import NoticeBoard from './Pages/NoticeBoard/noticeBoard';
import MapleHeader from './Components/MapleHeader';
import MapleFooter from './Components/MapleFooter';

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <MapleHeader />
        <Routes>
          <Route path="/test" element={<TestPage />} />
          <Route path="/write" element={<WritePage />} />
          <Route path="/notice" element={<NoticeBoard />} />
        </Routes>
        <MapleFooter
          github={'kimdg@github.com'}
          email={'kimdk3556@gmail.com'}
        />
      </BrowserRouter>
    </div>
  );
}

export default App;
