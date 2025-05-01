export default function AutoCompleteDropdown({
  suggestions = [],
  keyword = '',
  onSelect,
}) {
  if (!suggestions.length) return null;

  return (
    <ul className="absolute left-0 top-full w-full bg-white border border-[#A57865] mt-1 rounded shadow-md max-h-[200px] overflow-y-auto z-10">
      {suggestions.map((s, idx) => {
        const index = s.toLowerCase().indexOf(keyword.toLowerCase());
        let highlighted = s;

        if (index !== -1 && keyword.trim()) {
          const before = s.slice(0, index);
          const match = s.slice(index, index + keyword.length);
          const after = s.slice(index + keyword.length);
          highlighted = (
            <>
              {before}
              <mark className="bg-yellow-200 font-bold">{match}</mark>
              {after}
            </>
          );
        }

        return (
          <li
            key={idx}
            className="px-4 py-2 hover:bg-[#fdf2ea] cursor-pointer"
            onClick={() => onSelect(s)}
          >
            {highlighted}
          </li>
        );
      })}
    </ul>
  );
}
