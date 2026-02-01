

export function createContext(defaultValue) {
    const sym = Symbol(); // unique symbol

    function Context(props,children) {
       Reactor.setContextData(sym, props.value);
       return children;
    }

    Object.defineProperty(Context, "value", {
      get() { return Reactor.getContextData(sym, defaultValue); }
    });

    return Context;
}
