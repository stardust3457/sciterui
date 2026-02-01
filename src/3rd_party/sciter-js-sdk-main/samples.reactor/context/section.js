import { LevelContext } from './level-context.js';

export default class Section extends Element {

  render(props,kids) {
    let level = LevelContext.value + 1;
    return <section>
      <LevelContext value={level}>
        {kids}
      </LevelContext>
    </section>;
  }
}
