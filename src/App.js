import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import MapleFooter from './Components/MapleFooter';

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <Routes>
          <Route path="/test" element={<TestPage />} />
        </Routes>
        {/* 푸터 테스트 */}
        <MapleFooter
          github={'kimdg3556@gmail.com'}
          email={'kimdg3556@gmail.com'}
        />
      </BrowserRouter>
    </div>
  );
}

export default App;
