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
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
