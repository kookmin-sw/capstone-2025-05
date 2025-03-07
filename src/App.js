import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import MapleHeader from './Components/MapleHeader';

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <MapleHeader />
        <Routes>
          <Route path="/test" element={<TestPage />} />
          <Route path="/write" element={<WritePage />} />
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
